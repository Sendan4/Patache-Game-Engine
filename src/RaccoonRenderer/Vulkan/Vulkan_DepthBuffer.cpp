#include "Vulkan_DepthBuffer.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateDepthBuffer (
    YAML::Node & CONFIG)
{
  // Depth Image
  vk::FormatProperties2 FormatProperties
      = PhysicalDevice.getFormatProperties2 (vk::Format::eD16Unorm);

  vk::ImageTiling Tiling;
  if (FormatProperties.formatProperties.optimalTilingFeatures
      & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    {
      Tiling = vk::ImageTiling::eOptimal;
    }
  else if (FormatProperties.formatProperties.linearTilingFeatures
           & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    {
      Tiling = vk::ImageTiling::eLinear;
    }
  else
    {
      Patata::Log::FatalErrorMessage ("Patata - Raccoon Renderer",
                                      "DepthStencilAttachment is not "
                                      "supported for D16Unorm depth format",
                                      CONFIG);
      return false;
    }

  vk::ImageCreateInfo ImageInfo {};
  ImageInfo.sType = vk::StructureType::eImageCreateInfo;
  ImageInfo.pNext = nullptr;
  ImageInfo.imageType = vk::ImageType::e2D;
  ImageInfo.format    = vk::Format::eD16Unorm;
  ImageInfo.tiling    = Tiling;
  ImageInfo.usage     = vk::ImageUsageFlagBits::eDepthStencilAttachment;
  ImageInfo.samples   = vk::SampleCountFlagBits::e1;
  ImageInfo.extent    = vk::Extent3D (SwapChainExtent, 1);
  ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
  ImageInfo.sharingMode   = vk::SharingMode::eExclusive;
  ImageInfo.mipLevels     = 1;
  ImageInfo.arrayLayers   = 1;

  vk::Result Result = Device.createImage (&ImageInfo, nullptr, &DepthImage);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Depth Image", Result);
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

  vk::MemoryAllocateInfo DepthMemoryAllocateInfo {};
  DepthMemoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
  DepthMemoryAllocateInfo.pNext = nullptr;
  DepthMemoryAllocateInfo.memoryTypeIndex = TypeIndex;
  DepthMemoryAllocateInfo.allocationSize = MemoryRequirements.memoryRequirements.size;

  Result = Device.allocateMemory(&DepthMemoryAllocateInfo, nullptr, &DepthMemory);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Allocate Depth Memory", Result);
  }
  if (Result != vk::Result::eSuccess) return false;

  Device.bindImageMemory(DepthImage, DepthMemory, 0);

  vk::ComponentMapping Components;
  Components.r = vk::ComponentSwizzle::eZero;
  Components.g = vk::ComponentSwizzle::eOne;
  Components.b = vk::ComponentSwizzle::eZero;
  Components.a = vk::ComponentSwizzle::eOne;

  vk::ImageViewCreateInfo ImageDepthViewInfo {};
  ImageDepthViewInfo.sType = vk::StructureType::eImageViewCreateInfo;
  ImageDepthViewInfo.pNext = nullptr;
  //ImageDepthViewInfo.flags = ;
  ImageDepthViewInfo.image = DepthImage;
  ImageDepthViewInfo.viewType = vk::ImageViewType::e2D;
  ImageDepthViewInfo.format = vk::Format::eD16Unorm;
  ImageDepthViewInfo.components = Components;
  ImageDepthViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
  ImageDepthViewInfo.subresourceRange.baseArrayLayer = 0;
  ImageDepthViewInfo.subresourceRange.baseMipLevel = 0;
  ImageDepthViewInfo.subresourceRange.levelCount = 1;
  ImageDepthViewInfo.subresourceRange.layerCount = 1;

  Result = Device.createImageView(&ImageDepthViewInfo, nullptr, &DepthView);
  {
    std::future<void> ReturnVulkanCheck = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Depth Image View", Result);
  }
  if (Result != vk::Result::eSuccess) return false;
  else return true;
}
