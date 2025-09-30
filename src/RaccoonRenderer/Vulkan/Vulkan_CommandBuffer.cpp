#include "Vulkan_CommandBuffer.hpp"

bool
CreateCommandBuffer (Patache::VulkanBackend & rVulkan)
{
  const vk::CommandBufferAllocateInfo cmdAllocateInfo{
    .commandPool        = rVulkan.commandPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = rVulkan.swapchainImageCount,
  };

  if (rVulkan.pCmd == nullptr)
    rVulkan.pCmd = new vk::CommandBuffer[rVulkan.swapchainImageCount];

  vk::Result result = rVulkan.device.allocateCommandBuffers (&cmdAllocateInfo, rVulkan.pCmd);

  if (result != vk::Result::eSuccess)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "Allocate Command Buffer #%.3u",
                     rVulkan.currentFrame);

      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, errorText, result);

      return false;
    }

  return true;
}
