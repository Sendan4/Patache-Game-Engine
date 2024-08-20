#include "Vulkan_ImageView.hpp"

bool Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateImageView(const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> & SwapChainInfo) {
    fast_io::io::println(
#if defined(_WIN64)
        fast_io::out (),
#endif
        PATATA_TERM_BOLD,
        "\nSwapChain Color Image View : ",
        PATATA_TERM_RESET,
        SwapChainImageCount);

    SwapChainColorImageView = new vk::ImageView[SwapChainImageCount];

    vk::Result Result;

    for (uint8_t i = 0; i < SwapChainImageCount; ++i) {
        vk::ImageViewCreateInfo ColorImageViewInfo {};
        ColorImageViewInfo.sType = vk::StructureType::eImageViewCreateInfo;
        ColorImageViewInfo.pNext = nullptr;
        ColorImageViewInfo.image = SwapChainImages[i];
        ColorImageViewInfo.viewType = vk::ImageViewType::e2D;
        ColorImageViewInfo.format = std::get<1>(SwapChainInfo);
        ColorImageViewInfo.components.r = vk::ComponentSwizzle::eIdentity;
        ColorImageViewInfo.components.g = vk::ComponentSwizzle::eIdentity;
        ColorImageViewInfo.components.b = vk::ComponentSwizzle::eIdentity;
        ColorImageViewInfo.components.a = vk::ComponentSwizzle::eIdentity;
        ColorImageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        ColorImageViewInfo.subresourceRange.baseMipLevel = 0;
        ColorImageViewInfo.subresourceRange.levelCount = 1;
        ColorImageViewInfo.subresourceRange.baseArrayLayer = 0;
        ColorImageViewInfo.subresourceRange.layerCount = 1;

        Result = Device.createImageView(&ColorImageViewInfo, nullptr, &SwapChainColorImageView[i]);
        {
          fast_io::io::print (
#if defined(_WIN64)
              fast_io::out (),
#endif
              "  ");

          std::future<void> ReturnVulkanCheck = std::async (std::launch::async, Patata::Log::VulkanCheck,
                            "Color Image View", Result);
        }

        if (Result != vk::Result::eSuccess) return false;
    }

    return true;
}
