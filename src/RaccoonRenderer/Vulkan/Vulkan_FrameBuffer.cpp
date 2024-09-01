#include "Vulkan_FrameBuffer.hpp"

bool Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateFrameBuffer(void)
{
    fast_io::io::println (
    #if defined(_WIN64)
        fast_io::out (),
    #endif
        PATATA_TERM_BOLD,
        "\nSwapChain Frame Buffer : ",
        PATATA_TERM_RESET,
        SwapChainImageCount);

    SwapChainFrameBuffer = new vk::Framebuffer[SwapChainImageCount];

    vk::Result Result;

    for (uint8_t i = 0; i < SwapChainImageCount; ++i) {
        vk::FramebufferCreateInfo FrameBufferInfo {};
        FrameBufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        FrameBufferInfo.pNext = nullptr;
        FrameBufferInfo.renderPass = RenderPass;
        FrameBufferInfo.attachmentCount = 1;
        FrameBufferInfo.pAttachments = &SwapChainColorImageView[i];
        FrameBufferInfo.width = SwapChainExtent.width;
        FrameBufferInfo.height = SwapChainExtent.height;
        FrameBufferInfo.layers = 1;

        Result = Device.createFramebuffer(&FrameBufferInfo, nullptr, &SwapChainFrameBuffer[i]);
        {
            fast_io::io::print (
            #if defined(_WIN64)
                fast_io::out (),
            #endif
                "  ");

            std::future<void> ReturnVulkanCheck
                = std::async (std::launch::async, Patata::Log::VulkanCheck, "Frame Buffer", Result);
        }

        if (Result != vk::Result::eSuccess) return false;
    }

    fast_io::io::println (
    #if defined(_WIN64)
        fast_io::out (),
    #endif
        "");

    return true;
}
