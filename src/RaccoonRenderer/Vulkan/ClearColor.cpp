#include <vulkan/vulkan.hpp>

#include "RaccoonRenderer.hpp"

void Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanClearColor(
    const float & R, const float & G, const float & B, const float & A) {
    if(!CmdIsReady) return;

    vk::ImageMemoryBarrier InitialBarrier {};
    InitialBarrier.sType = vk::StructureType::eImageMemoryBarrier;
    InitialBarrier.oldLayout = vk::ImageLayout::eUndefined;
    InitialBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
    InitialBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    InitialBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    InitialBarrier.image = SwapChainImages[ImageIndex];
    InitialBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    InitialBarrier.subresourceRange.baseMipLevel = 0;
    InitialBarrier.subresourceRange.levelCount = 1;
    InitialBarrier.subresourceRange.baseArrayLayer = 0;
    InitialBarrier.subresourceRange.layerCount = 1;

    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &InitialBarrier);

    const vk::ClearColorValue Color { R, G, B, A };

    vk::ImageSubresourceRange ImageSubresourceRange {};
    ImageSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    ImageSubresourceRange.levelCount = 1;
    ImageSubresourceRange.layerCount = 1;

    cmd.clearColorImage(SwapChainImages[ImageIndex], vk::ImageLayout::eTransferDstOptimal, &Color, 1, &ImageSubresourceRange);

    vk::ImageMemoryBarrier FinalBarrier {};
    FinalBarrier.sType = vk::StructureType::eImageMemoryBarrier;
    FinalBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    FinalBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
    FinalBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    FinalBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    FinalBarrier.image = SwapChainImages[ImageIndex];
    FinalBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    FinalBarrier.subresourceRange.baseMipLevel = 0;
    FinalBarrier.subresourceRange.levelCount = 1;
    FinalBarrier.subresourceRange.baseArrayLayer = 0;
    FinalBarrier.subresourceRange.layerCount = 1;

    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &FinalBarrier);
}
