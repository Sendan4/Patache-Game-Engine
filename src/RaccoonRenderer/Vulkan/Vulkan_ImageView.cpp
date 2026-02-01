#include "Vulkan_ImageView.hpp"

bool
CreateImageView (Patache::VulkanBackend & rVulkan, const Patache::SwapchainInfo & rSwapchainInfo)
{
  if (rVulkan.pSwapchainColorImageViews == nullptr)
    rVulkan.pSwapchainColorImageViews = new vk::ImageView[rVulkan.swapchainImageCount];

  vk::Result result;

  constexpr vk::ComponentMapping componentMapping{ .r = vk::ComponentSwizzle::eIdentity,
                                                   .g = vk::ComponentSwizzle::eIdentity,
                                                   .b = vk::ComponentSwizzle::eIdentity,
                                                   .a = vk::ComponentSwizzle::eIdentity };

  constexpr vk::ImageSubresourceRange imageSubresourceRange{ .aspectMask
                                                             = vk::ImageAspectFlagBits::eColor,
                                                             .baseMipLevel   = 0U,
                                                             .levelCount     = 1U,
                                                             .baseArrayLayer = 0U,
                                                             .layerCount     = 1U };

  for (std::uint8_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      const vk::ImageViewCreateInfo colorImageViewInfo{ .image    = rVulkan.pSwapchainImages[i],
                                                        .viewType = vk::ImageViewType::e2D,
                                                        .format   = rSwapchainInfo.imageColorFormat,
                                                        .components       = componentMapping,
                                                        .subresourceRange = imageSubresourceRange };

      result = rVulkan.device.createImageView (&colorImageViewInfo, nullptr,
                                               &rVulkan.pSwapchainColorImageViews[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateImageView() Color Image View #%.3u", i + 1);

          std::future<void> returnrVulkanCheck{ std::async (
              std::launch::async, Patache::VulkanCheck, errorText, result) };

          return false;
        }
    }

  return true;
}
