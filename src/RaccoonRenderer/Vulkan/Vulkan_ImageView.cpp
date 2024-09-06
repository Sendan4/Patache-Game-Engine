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
        vk::ImageViewCreateInfo ColorImageViewInfo ({},
            SwapChainImages[i],         // image
            vk::ImageViewType::e2D,     // viewType
            std::get<1>(SwapChainInfo), // format
            // Component Mapping
            {
                vk::ComponentSwizzle::eIdentity, // R
                vk::ComponentSwizzle::eIdentity, // G
                vk::ComponentSwizzle::eIdentity, // B
                vk::ComponentSwizzle::eIdentity  // A
            },
            // Subresource Range
            {
                vk::ImageAspectFlagBits::eColor, // aspectMask
                0,                               // baseMipLevel
                1,                               // levelCount
                0,                               // baseArrayLayer
                1                                // layerCount
            },
            nullptr // pNext
        );

        Result = Device.createImageView(&ColorImageViewInfo, nullptr, &SwapChainColorImageView[i]);
        {
          fast_io::io::print (
            #if defined(_WIN64)
            fast_io::out (),
            #endif
            "  ");

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patata::Log::VulkanCheck, "Color Image View", Result);
        }

        if (Result != vk::Result::eSuccess) return false;
    }

    return true;
}
