#include "Vulkan_DepthBuffer.hpp"

bool
CreateDepthBuffer (Patache::VulkanBackend & rVulkan, Patache::Config & rConfiguration)
{
  VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };

  constexpr VkFormat depthFormatsToCheck[4U]{ VK_FORMAT_D32_SFLOAT_S8_UINT, // Priority
                                              VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT,
                                              VK_FORMAT_D16_UNORM };

  VkFormat selectedDepthFormat{ VK_FORMAT_UNDEFINED };
  bool     found{ false };

  // Depth Image And Image Tiling
  VkFormatProperties2 formatProperties{};

  for (std::uint8_t i{ 0U }; i < 4U; ++i)
    {
      vkGetPhysicalDeviceFormatProperties2 (rVulkan.physicalDevice, depthFormatsToCheck[i],
                                            &formatProperties);

      if (formatProperties.formatProperties.optimalTilingFeatures
          & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) // Priority
        {
          selectedDepthFormat = depthFormatsToCheck[i];
          found               = true;
          break;
        }
      else if (formatProperties.formatProperties.linearTilingFeatures
               & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
          tiling              = VK_IMAGE_TILING_LINEAR;
          selectedDepthFormat = depthFormatsToCheck[i];
          found               = true;
          break;
        }
    }

  if (!found)
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache - Raccoon Renderer",
          "A ImageTiling or Depth Format was not found", rConfiguration) };

      return false;
    }

  const VkImageCreateInfo imageInfo{ .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                     .pNext     = nullptr,
                                     .flags     = 0U,
                                     .imageType = VK_IMAGE_TYPE_2D,
                                     .format    = selectedDepthFormat,
                                     .extent{ .width  = rVulkan.swapchainExtent.width,
                                              .height = rVulkan.swapchainExtent.height,
                                              .depth  = 1U },
                                     .mipLevels   = 1U,
                                     .arrayLayers = 1U,
                                     .samples     = VK_SAMPLE_COUNT_1_BIT,
                                     .tiling      = tiling,
                                     .usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                     .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                     .queueFamilyIndexCount = 0U,
                                     .pQueueFamilyIndices   = nullptr,
                                     .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED };

  VkResult result{ vkCreateImage (rVulkan.device, &imageInfo, nullptr, &rVulkan.depthImage) };

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkCreateImage()", result) };

      return false;
    }

  // Depth View
  VkPhysicalDeviceMemoryProperties2 memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties2 (rVulkan.physicalDevice, &memoryProperties);

  VkMemoryRequirements  mr;
  VkMemoryRequirements2 memoryRequirements{};
  memoryRequirements.memoryRequirements = mr;

  VkImageMemoryRequirementsInfo2 imageMemoryRequirementInfo{};
  imageMemoryRequirementInfo.image = rVulkan.depthImage;

  vkGetImageMemoryRequirements2 (rVulkan.device, &imageMemoryRequirementInfo, &memoryRequirements);

  std::uint32_t typeIndex{ 0U };

  for (std::uint32_t i{ 0U }; i < memoryProperties.memoryProperties.memoryTypeCount; ++i)
    {
      if ((memoryRequirements.memoryRequirements.memoryTypeBits & 1U)
          && (memoryProperties.memoryProperties.memoryTypes[i].propertyFlags
              & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
          typeIndex = i;
          break;
        }
    }

  const VkMemoryAllocateInfo depthMemoryAllocateInfo{
    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext           = nullptr,
    .allocationSize  = memoryRequirements.memoryRequirements.size,
    .memoryTypeIndex = typeIndex,
  };

  result
      = vkAllocateMemory (rVulkan.device, &depthMemoryAllocateInfo, nullptr, &rVulkan.depthMemory);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkAllocateMemory()", result) };

      return false;
    }

  result = vkBindImageMemory (rVulkan.device, rVulkan.depthImage, rVulkan.depthMemory, 0U);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkBindImageMemory()", result) };

      return false;
    }

  const VkImageViewCreateInfo imageDepthViewInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = 0U,
                                                  .image = rVulkan.depthImage,
                                                  .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                                  .format   = selectedDepthFormat,
                                                  .components{ .r = VK_COMPONENT_SWIZZLE_R,
                                                               .g = VK_COMPONENT_SWIZZLE_G,
                                                               .b = VK_COMPONENT_SWIZZLE_B,
                                                               .a = VK_COMPONENT_SWIZZLE_A },
                                                  .subresourceRange{ .aspectMask
                                                                     = VK_IMAGE_ASPECT_DEPTH_BIT,
                                                                     .baseMipLevel   = 0U,
                                                                     .levelCount     = 1U,
                                                                     .baseArrayLayer = 0U,
                                                                     .layerCount     = 1U } };

  result = vkCreateImageView (rVulkan.device, &imageDepthViewInfo, nullptr, &rVulkan.depthView);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkCreateImageView()", result) };

      return false;
    }

  return true;
}
