#include "Vulkan_ImageView.hpp"

bool Patata::RaccoonRenderer::CreateImageView(
    const Patata::SwapChainInfo & SwapChainInfo) {
    Vulkan.SwapChainColorImageView = new vk::ImageView[Vulkan.SwapChainImageCount];

    vk::Result Result;

    for (uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i) {
        vk::ImageViewCreateInfo ColorImageViewInfo ({},
            Vulkan.SwapChainImages[i],      // image
            vk::ImageViewType::e2D,         // viewType
            SwapChainInfo.ImageColorFormat, // format
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

        Result = Vulkan.Device.createImageView(&ColorImageViewInfo, nullptr, &Vulkan.SwapChainColorImageView[i]);
        if (Result != vk::Result::eSuccess)
          {
          uint8_t           tmp               = i;

            std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
                Patata::Log::VulkanCheck,
                std::string ("Color Image View " + std::to_string (++tmp) + '/' + std::to_string (Vulkan.SwapChainImageCount)),
                Result);

            return false;
        }
    }
    return true;
}
