#include "Vulkan_ImageView.hpp"

bool
Patache::Engine::CreateImageView (const Patache::SwapChainInfo & SwapChainInfo)
{
  if (Vulkan.SwapChainColorImageView == VK_NULL_HANDLE)
    Vulkan.SwapChainColorImageView
        = new vk::ImageView[Vulkan.SwapChainImageCount];

  vk::Result Result;

  static constexpr vk::ComponentMapping ComponentMapping{
    .r = vk::ComponentSwizzle::eIdentity,
    .g = vk::ComponentSwizzle::eIdentity,
    .b = vk::ComponentSwizzle::eIdentity,
    .a = vk::ComponentSwizzle::eIdentity
  };

  static constexpr vk::ImageSubresourceRange ImageSubresourceRange{
    .aspectMask     = vk::ImageAspectFlagBits::eColor,
    .baseMipLevel   = 0,
    .levelCount     = 1,
    .baseArrayLayer = 0,
    .layerCount     = 1
  };

  for (std::uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      vk::ImageViewCreateInfo ColorImageViewInfo{
        .image            = Vulkan.SwapChainImages[i],
        .viewType         = vk::ImageViewType::e2D,
        .format           = SwapChainInfo.ImageColorFormat,
        .components       = ComponentMapping,
        .subresourceRange = ImageSubresourceRange
      };

      Result = Vulkan.Device.createImageView (
          &ColorImageViewInfo, nullptr, &Vulkan.SwapChainColorImageView[i]);

      if (Result != vk::Result::eSuccess)
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "Color Image View #%.3u", i + 1);

          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            ErrorText, Result);

          return false;
        }
    }

  return true;
}
