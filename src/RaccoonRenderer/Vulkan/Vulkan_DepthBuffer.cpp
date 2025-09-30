#include "Vulkan_DepthBuffer.hpp"

bool
CreateDepthBuffer (Patache::VulkanBackend & rVulkan, Patache::Config & rConfiguration)
{
  vk::ImageTiling tiling = vk::ImageTiling::eOptimal;

  constexpr vk::Format depthFormatsToCheck[4]
      = { vk::Format::eD32SfloatS8Uint, // Priority
          vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16Unorm };

  vk::Format selectedDepthFormat = vk::Format::eUndefined;
  bool       found               = false;

  // Depth Image And Image Tiling
  for (std::uint8_t i = 0; i < 4; ++i)
    {
      vk::FormatProperties2 formatProperties
          = rVulkan.physicalDevice.getFormatProperties2 (depthFormatsToCheck[i]);

      if (formatProperties.formatProperties.optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eDepthStencilAttachment) // Priority
        {
          tiling              = vk::ImageTiling::eOptimal;
          selectedDepthFormat = depthFormatsToCheck[i];
          found               = true;
          break;
        }
      else if (formatProperties.formatProperties.linearTilingFeatures
               & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
          tiling              = vk::ImageTiling::eLinear;
          selectedDepthFormat = depthFormatsToCheck[i];
          found               = true;
          break;
        }
    }

  if (!found)
    {
      std::future<void> err = std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache - Raccoon Renderer",
          "A ImageTiling or Depth Format was not found", rConfiguration);

      return false;
    }

  const vk::ImageCreateInfo imageInfo{ .imageType   = vk::ImageType::e2D,
                                       .format      = selectedDepthFormat,
                                       .extent      = vk::Extent3D{ rVulkan.swapchainExtent.width,
                                                               rVulkan.swapchainExtent.height, 1 },
                                       .mipLevels   = 1,
                                       .arrayLayers = 1,
                                       .samples     = vk::SampleCountFlagBits::e1,
                                       .tiling      = tiling,
                                       .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                       .sharingMode           = vk::SharingMode::eExclusive,
                                       .queueFamilyIndexCount = 0,
                                       .initialLayout         = vk::ImageLayout::eUndefined };

  vk::Result result = rVulkan.device.createImage (&imageInfo, nullptr, &rVulkan.depthImage);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Depth Image", result);

      return false;
    }

  // Depth View
  const vk::PhysicalDeviceMemoryProperties2 memoryProperties
      = rVulkan.physicalDevice.getMemoryProperties2 ();

  vk::MemoryRequirements  mr;
  vk::MemoryRequirements2 memoryRequirements{ .memoryRequirements = mr };

  vk::ImageMemoryRequirementsInfo2 imageMemoryRequirementInfo{ .image = rVulkan.depthImage };

  rVulkan.device.getImageMemoryRequirements2 (&imageMemoryRequirementInfo, &memoryRequirements);

  std::uint32_t typeIndex = 0;

  for (std::uint32_t i = 0; i < memoryProperties.memoryProperties.memoryTypeCount; ++i)
    {
      if ((memoryRequirements.memoryRequirements.memoryTypeBits & 1)
          && (memoryProperties.memoryProperties.memoryTypes[i].propertyFlags
              & vk::MemoryPropertyFlagBits::eDeviceLocal))
        {
          typeIndex = i;
          break;
        }
    }

  const vk::MemoryAllocateInfo depthMemoryAllocateInfo{
    .allocationSize  = memoryRequirements.memoryRequirements.size,
    .memoryTypeIndex = typeIndex,
  };

  result = rVulkan.device.allocateMemory (&depthMemoryAllocateInfo, nullptr, &rVulkan.depthMemory);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Allocate Depth Memory", result);

      return false;
    }

  result = rVulkan.device.bindImageMemory (rVulkan.depthImage, rVulkan.depthMemory, 0);
  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck = std::async (std::launch::async, Patache::VulkanCheck,
                                                        "Bind Image Depth Memory", result);

      return false;
    }

  const vk::ImageViewCreateInfo imageDepthViewInfo{
    .image      = rVulkan.depthImage,
    .viewType   = vk::ImageViewType::e2D,
    .format     = selectedDepthFormat,
    .components = {},
    .subresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                       .baseMipLevel   = 0,
                       .levelCount     = 1,
                       .baseArrayLayer = 0,
                       .layerCount     = 1 }
  };

  result = rVulkan.device.createImageView (&imageDepthViewInfo, nullptr, &rVulkan.depthView);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Depth Image View", result);

      return false;
    }
  else
    return true;
}
