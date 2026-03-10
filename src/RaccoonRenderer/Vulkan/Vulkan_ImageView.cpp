#include "Vulkan_ImageView.hpp"

bool
CreateImageView (Patache::VulkanBackend & rVulkan, const Patache::SwapchainInfo & rSwapchainInfo)
{
  rVulkan.pSwapchainColorImageViews = static_cast<VkImageView *> (
      std::malloc (sizeof (VkImageView) * rVulkan.swapchainImageCount));

  if (rVulkan.pSwapchainColorImageViews == nullptr)
    {
      return false;
    }

  VkResult result;

  VkImageViewCreateInfo colorImageViewInfo{ .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                            .pNext    = nullptr,
                                            .flags    = 0U,
                                            .image    = VK_NULL_HANDLE,
                                            .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                            .format   = rSwapchainInfo.imageColorFormat,
                                            .components{ .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                         .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                         .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                         .a = VK_COMPONENT_SWIZZLE_IDENTITY },
                                            .subresourceRange{ .aspectMask
                                                               = VK_IMAGE_ASPECT_COLOR_BIT,
                                                               .baseMipLevel   = 0U,
                                                               .levelCount     = 1U,
                                                               .baseArrayLayer = 0U,
                                                               .layerCount     = 1U } };

  for (std::uint8_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      colorImageViewInfo.image = rVulkan.pSwapchainImages[i];

      result = vkCreateImageView (rVulkan.device, &colorImageViewInfo, nullptr,
                                  &rVulkan.pSwapchainColorImageViews[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateImageView() Color Image View #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }
    }

  return true;
}
