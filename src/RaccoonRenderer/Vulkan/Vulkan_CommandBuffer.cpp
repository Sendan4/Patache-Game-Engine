#include "Vulkan_CommandBuffer.hpp"

bool
CreateCommandBuffer (Patache::VulkanBackend & rVulkan)
{
  vk::CommandBufferAllocateInfo cmdAllocateInfo{ .level = vk::CommandBufferLevel::ePrimary,
                                                 .commandBufferCount = 1 };

  if (rVulkan.pCmd == nullptr)
    {
      rVulkan.pCmd = static_cast<vk::CommandBuffer *> (
          std::malloc (sizeof (vk::CommandBuffer) * rVulkan.swapchainImageCount));
    }

  vk::Result result;

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      // Render command buffer
      cmdAllocateInfo.commandPool = rVulkan.pCommandPools[i];

      result = rVulkan.device.allocateCommandBuffers (&cmdAllocateInfo, &rVulkan.pCmd[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkAllocateCommandBuffers() Primary Render Command Buffer #%.3u", i);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }
    }

  return true;
}
