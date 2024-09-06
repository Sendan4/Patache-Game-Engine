#include "Vulkan_DepthBuffer.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateDepthBuffer (void)
{
  fast_io::io::println (
    #if defined(_WIN64)
    fast_io::out (),
    #endif
    "");

  vk::ImageTiling Tiling;

  vk::Format DepthFormatsToCheck[4] = {
      vk::Format::eD32SfloatS8Uint, // Priority
      vk::Format::eD32Sfloat,
      vk::Format::eD24UnormS8Uint,
      vk::Format::eD16Unorm
  };

  vk::Format SelectedDepthFormat;
  bool Found = false;

  // Depth Image And Image Tiling
  for (uint8_t i = 0; i < 4; ++i) {
      vk::FormatProperties2 FormatProperties
          = PhysicalDevice.getFormatProperties2 (DepthFormatsToCheck[i]);

      if (FormatProperties.formatProperties.optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eDepthStencilAttachment) // Priority
        {
          Tiling = vk::ImageTiling::eOptimal;
          SelectedDepthFormat = DepthFormatsToCheck[i];
          Found = true;
          break;
        }
      else if (FormatProperties.formatProperties.linearTilingFeatures
               & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
          Tiling = vk::ImageTiling::eLinear;
          SelectedDepthFormat = DepthFormatsToCheck[i];
          Found = true;
          break;
        }
  }

  if (!Found) {
      Patata::Log::FatalErrorMessage ("Patata - Raccoon Renderer",
                                      "A ImageTiling or Depth Format was not found",
                                      *pConfiguration);
      return false;
  }

  vk::ImageCreateInfo ImageInfo ({},
      vk::ImageType::e2D,                              // imageType
      SelectedDepthFormat,                             // format
      vk::Extent3D (SwapChainExtent, 1),               // extent
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

  vk::Result Result = Device.createImage (&ImageInfo, nullptr, &DepthImage);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
        Patata::Log::VulkanCheck, "Depth Image", Result);
  }
  if (Result != vk::Result::eSuccess) return false;

  // Depth View
  vk::PhysicalDeviceMemoryProperties2 MemoryProperties = PhysicalDevice.getMemoryProperties2();
  vk::MemoryRequirements2 MemoryRequirements = Device.getImageMemoryRequirements2(DepthImage);
  uint32_t TypeIndex = 0;

  for (uint32_t i = 0; i < MemoryProperties.memoryProperties.memoryTypeCount; ++i) {
    if ((MemoryRequirements.memoryRequirements.memoryTypeBits & 1)
        && (MemoryProperties.memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        TypeIndex = i;
        break;
    }
  }

  vk::MemoryAllocateInfo DepthMemoryAllocateInfo (
      MemoryRequirements.memoryRequirements.size, // allocationSize
      TypeIndex,                                  // memoryTypeIndex
      nullptr                                     // pNext
  );

  Result = Device.allocateMemory(&DepthMemoryAllocateInfo, nullptr, &DepthMemory);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
        Patata::Log::VulkanCheck, "Allocate Depth Memory", Result);
  }
  if (Result != vk::Result::eSuccess) return false;

  Result = Device.bindImageMemory(DepthImage, DepthMemory, 0);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
        Patata::Log::VulkanCheck, "Bind Image Depth Memory", Result);
  }

  vk::ComponentMapping Components;

  vk::ImageViewCreateInfo ImageDepthViewInfo({},
      DepthImage,             // image
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
      }
  );

  Result = Device.createImageView(&ImageDepthViewInfo, nullptr, &DepthView);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
        Patata::Log::VulkanCheck, "Depth Image View", Result);
  }
  if (Result != vk::Result::eSuccess) return false;
  else return true;
}
