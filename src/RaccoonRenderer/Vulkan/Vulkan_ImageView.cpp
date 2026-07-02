#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

namespace Patache
{
static constexpr std::uint8_t sErrorTextSize{ 64U };
static constexpr std::uint8_t sErrorTextSizeExtraNull{ 65U };
}

#include "Vulkan_ImageView.hpp"

bool
Patache::CreateImageView (Patache::VulkanBackend &       rVulkan,
                          const Patache::SwapchainInfo & rSwapchainInfo)
{
  if (rVulkan.pSwapchainColorImageViews == nullptr)
    {
      rVulkan.pSwapchainColorImageViews = static_cast<VkImageView *> (
          std::calloc (rVulkan.swapchainImageCount, sizeof (VkImageView)));

      if (rVulkan.pSwapchainColorImageViews == nullptr)
        {
          return false;
        }
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
          char errorText[Patache::sErrorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::sErrorTextSize,
                         "vkCreateImageView() Color Image View #%.3u", i + 1);

          Patache::VulkanCheck (errorText, result);

          return false;
        }
    }

  return true;
}
