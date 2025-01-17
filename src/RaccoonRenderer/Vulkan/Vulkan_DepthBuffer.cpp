#include "Vulkan_DepthBuffer.hpp"

bool
Patata::Engine::CreateDepthBuffer (void)
{
  vk::ImageTiling Tiling = vk::ImageTiling::eOptimal;

  vk::Format DepthFormatsToCheck[4]
      = { vk::Format::eD32SfloatS8Uint, // Priority
          vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint,
          vk::Format::eD16Unorm };

  vk::Format SelectedDepthFormat = vk::Format::eUndefined;
  bool       Found               = false;

  // Depth Image And Image Tiling
  for (uint8_t i = 0; i < 4; ++i)
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
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata - Raccoon Renderer",
          "A ImageTiling or Depth Format was not found", configuration);

      return false;
    }

  vk::ImageCreateInfo ImageInfo (
      {},
      vk::ImageType::e2D,                              // imageType
      SelectedDepthFormat,                             // format
      vk::Extent3D (Vulkan.SwapChainExtent, 1),        // extent
      1,                                               // mipLevels
      1,                                               // arrayLayers
      vk::SampleCountFlagBits::e1,                     // samples
      Tiling,                                          // tiling
      vk::ImageUsageFlagBits::eDepthStencilAttachment, // usage
      vk::SharingMode::eExclusive,                     // sharingMode
      0,                                               // queueFamilyIndexCount
      nullptr,                                         // pQueueFamilyIndices
      vk::ImageLayout::eUndefined,                     // initialLayout
      nullptr                                          // pNext
  );

  vk::Result Result
      = Vulkan.Device.createImage (&ImageInfo, nullptr, &Vulkan.DepthImage);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Depth Image", Result);

      return false;
    }

  // Depth View
  vk::PhysicalDeviceMemoryProperties2 MemoryProperties
      = Vulkan.PhysicalDevice.getMemoryProperties2 ();
  vk::MemoryRequirements2 MemoryRequirements
      = Vulkan.Device.getImageMemoryRequirements2 (Vulkan.DepthImage);
  uint32_t TypeIndex = 0;

  for (uint32_t i = 0; i < MemoryProperties.memoryProperties.memoryTypeCount;
       ++i)
    {
      if ((MemoryRequirements.memoryRequirements.memoryTypeBits & 1)
          && (MemoryProperties.memoryProperties.memoryTypes[i].propertyFlags
              & vk::MemoryPropertyFlagBits::eDeviceLocal))
        {
          TypeIndex = i;
          break;
        }
    }

  vk::MemoryAllocateInfo DepthMemoryAllocateInfo (
      MemoryRequirements.memoryRequirements.size, // allocationSize
      TypeIndex,                                  // memoryTypeIndex
      nullptr                                     // pNext
  );

  Result = Vulkan.Device.allocateMemory (&DepthMemoryAllocateInfo, nullptr,
                                         &Vulkan.DepthMemory);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Allocate Depth Memory", Result);

      return false;
    }

  Result = Vulkan.Device.bindImageMemory (Vulkan.DepthImage,
                                          Vulkan.DepthMemory, 0);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Bind Image Depth Memory", Result);

      return false;
    }

  vk::ComponentMapping Components;

  vk::ImageViewCreateInfo ImageDepthViewInfo (
      {},
      Vulkan.DepthImage,      // image
      vk::ImageViewType::e2D, // viewType
      SelectedDepthFormat,    // format
      Components,             // components
      // subresourceRange
      {
          vk::ImageAspectFlagBits::eDepth, // aspectMask
          0,                               // baseMipLevel
          1,                               // levelCount
          0,                               // baseArrayLayer
          1                                // layerCount
      });

  Result = Vulkan.Device.createImageView (&ImageDepthViewInfo, nullptr,
                                          &Vulkan.DepthView);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Depth Image View", Result);

      return false;
    }
  else
    return true;
}
