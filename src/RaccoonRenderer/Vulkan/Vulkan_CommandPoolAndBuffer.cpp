#include "Vulkan_CommandPoolAndBuffer.hpp"

bool
Patata::RaccoonRenderer::CreateCommandPool (void)
{
  vk::CommandPoolCreateInfo CommandPoolInfo (
      vk::CommandPoolCreateFlagBits::eTransient                 // flags
          | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, // flags
      Vulkan.GraphicsQueueFamilyIndex, // queueFamilyIndex
      nullptr                          // pNext
  );

  vk::Result Result = Vulkan.Device.createCommandPool (
      &CommandPoolInfo, nullptr, &Vulkan.CommandPool);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Command Pool", Result);
      return false;
    }
  return true;
}

bool
Patata::RaccoonRenderer::CreateCommandBuffer (void)
{
  vk::CommandBufferAllocateInfo cmdAllocateInfo (
      Vulkan.CommandPool,               // commandPool
      vk::CommandBufferLevel::ePrimary, // level
      1,                                // commandBufferCount
      nullptr                           // pNext
  );

  vk::Result Result
      = Vulkan.Device.allocateCommandBuffers (&cmdAllocateInfo, &Vulkan.cmd);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Allocate Command Buffer", Result);

      return false;
    }
  return true;
}
