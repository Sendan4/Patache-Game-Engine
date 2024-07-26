#include "Vulkan_CommandPool.hpp"

void
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateCommandPool (
    uint32_t & GraphicsQueueFamilyIndex)
{
  vk::CommandPoolCreateInfo CreateCommandPoolInfo {};
  CreateCommandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
  CreateCommandPoolInfo.pNext = nullptr;
  CreateCommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
  CreateCommandPoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

  vk::Result Result = Device.createCommandPool (&CreateCommandPoolInfo,
                                                nullptr, &CommandPool);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Command Pool", Result);
  }
}
