#include "Vulkan_CommandPoolAndBuffer.hpp"

bool
Patata::Engine::CreateCommandPool (void)
{
  vk::CommandPoolCreateInfo CommandPoolInfo{
    .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = Vulkan.GraphicsQueueFamilyIndex,
  };

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
Patata::Engine::CreateCommandBuffer (void)
{
  vk::CommandBufferAllocateInfo cmdAllocateInfo{
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
      char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                     "Allocate Command Buffer #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, ErrorText, Result);

      return false;
    }

  return true;
}
