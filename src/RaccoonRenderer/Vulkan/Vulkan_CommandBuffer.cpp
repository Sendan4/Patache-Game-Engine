#include "Vulkan_CommandBuffer.hpp"

bool
CreateCommandBuffer (Patache::VulkanBackend & Vulkan)
{
  const vk::CommandBufferAllocateInfo cmdAllocateInfo{
    .commandPool        = Vulkan.CommandPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = Vulkan.SwapChainImageCount,
  };

  if (Vulkan.Cmd == VK_NULL_HANDLE)
    Vulkan.Cmd = new vk::CommandBuffer[Vulkan.SwapChainImageCount];

  vk::Result Result
      = Vulkan.Device.allocateCommandBuffers (&cmdAllocateInfo, Vulkan.Cmd);

  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "Allocate Command Buffer #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, ErrorText, Result);

      return false;
    }

  return true;
}
