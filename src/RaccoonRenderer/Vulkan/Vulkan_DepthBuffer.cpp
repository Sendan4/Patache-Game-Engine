#include "Vulkan_DepthBuffer.hpp"

bool
CreateDepthBuffer (Patache::VulkanBackend & Vulkan,
                   Patache::Config &        configuration)
{
  vk::ImageTiling Tiling = vk::ImageTiling::eOptimal;

  vk::Format DepthFormatsToCheck[4]
      = { vk::Format::eD32SfloatS8Uint, // Priority
          vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint,
          vk::Format::eD16Unorm };

  vk::Format SelectedDepthFormat = vk::Format::eUndefined;
  bool       Found               = false;

  // Depth Image And Image Tiling
  for (std::uint8_t i = 0; i < 4; ++i)
    {
      vk::FormatProperties2 FormatProperties
          = Vulkan.PhysicalDevice.getFormatProperties2 (
              DepthFormatsToCheck[i]);

      if (FormatProperties.formatProperties.optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eDepthStencilAttachment) // Priority
        {
          Tiling              = vk::ImageTiling::eOptimal;
          SelectedDepthFormat = DepthFormatsToCheck[i];
          Found               = true;
          break;
        }
      else if (FormatProperties.formatProperties.linearTilingFeatures
               & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
          Tiling              = vk::ImageTiling::eLinear;
          SelectedDepthFormat = DepthFormatsToCheck[i];
          Found               = true;
          break;
        }
    }

  if (!Found)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache - Raccoon Renderer",
          "A ImageTiling or Depth Format was not found",
          configuration);

      return false;
    }

  vk::Extent3D Extent{ .width  = Vulkan.SwapChainExtent.width,
                       .height = Vulkan.SwapChainExtent.height,
                       .depth  = 1 };

  vk::ImageCreateInfo ImageInfo{
    .imageType             = vk::ImageType::e2D,
    .format                = SelectedDepthFormat,
    .extent                = Extent,
    .mipLevels             = 1,
    .arrayLayers           = 1,
    .samples               = vk::SampleCountFlagBits::e1,
    .tiling                = Tiling,
    .usage                 = vk::ImageUsageFlagBits::eDepthStencilAttachment,
    .sharingMode           = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount = 0,
    .initialLayout         = vk::ImageLayout::eUndefined
  };

  vk::Result Result
      = Vulkan.Device.createImage (&ImageInfo, nullptr, &Vulkan.DepthImage);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Depth Image", Result);

      return false;
    }

  // Depth View
  vk::PhysicalDeviceMemoryProperties2 MemoryProperties
      = Vulkan.PhysicalDevice.getMemoryProperties2 ();

  vk::MemoryRequirements  mr;
  vk::MemoryRequirements2 MemoryRequirements{ .memoryRequirements = mr };

  vk::ImageMemoryRequirementsInfo2 ImageMemoryRequirementInfo{
    .image = Vulkan.DepthImage
  };

  Vulkan.Device.getImageMemoryRequirements2 (&ImageMemoryRequirementInfo,
                                             &MemoryRequirements);

  std::uint32_t TypeIndex = 0;

  for (std::uint32_t i = 0;
       i < MemoryProperties.memoryProperties.memoryTypeCount; ++i)
    {
      if ((MemoryRequirements.memoryRequirements.memoryTypeBits & 1)
          && (MemoryProperties.memoryProperties.memoryTypes[i].propertyFlags
              & vk::MemoryPropertyFlagBits::eDeviceLocal))
        {
          TypeIndex = i;
          break;
        }
    }

  vk::MemoryAllocateInfo DepthMemoryAllocateInfo{
    .allocationSize  = MemoryRequirements.memoryRequirements.size,
    .memoryTypeIndex = TypeIndex,
  };

  Result = Vulkan.Device.allocateMemory (&DepthMemoryAllocateInfo, nullptr,
                                         &Vulkan.DepthMemory);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Allocate Depth Memory", Result);

      return false;
    }

  Result = Vulkan.Device.bindImageMemory (Vulkan.DepthImage,
                                          Vulkan.DepthMemory, 0);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Bind Image Depth Memory", Result);

      return false;
    }

  vk::ComponentMapping Components{};

  vk::ImageViewCreateInfo ImageDepthViewInfo{
    .image      = Vulkan.DepthImage,
    .viewType   = vk::ImageViewType::e2D,
    .format     = SelectedDepthFormat,
    .components = Components,
    .subresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                       .baseMipLevel   = 0,
                       .levelCount     = 1,
                       .baseArrayLayer = 0,
                       .layerCount     = 1 }
  };

  Result = Vulkan.Device.createImageView (&ImageDepthViewInfo, nullptr,
                                          &Vulkan.DepthView);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Depth Image View", Result);

      return false;
    }
  else
    return true;
}
