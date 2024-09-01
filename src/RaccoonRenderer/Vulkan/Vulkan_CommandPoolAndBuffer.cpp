#include "Vulkan_CommandPoolAndBuffer.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateCommandPool (void) {
    vk::CommandPoolCreateInfo CommandPoolInfo {};
    CommandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    CommandPoolInfo.pNext = nullptr;
    CommandPoolInfo.flags
        = vk::CommandPoolCreateFlagBits::eTransient
          | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

    vk::Result Result = Device.createCommandPool (&CommandPoolInfo,
                                                  nullptr, &CommandPool);
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Command Pool", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateCommandBuffer (void)
{
    vk::CommandBufferAllocateInfo cmdAllocateInfo {};
    cmdAllocateInfo.sType       = vk::StructureType::eCommandBufferAllocateInfo;
    cmdAllocateInfo.pNext       = nullptr;
    cmdAllocateInfo.commandPool = CommandPool;
    cmdAllocateInfo.level       = vk::CommandBufferLevel::ePrimary;
    cmdAllocateInfo.commandBufferCount = 1;

    vk::Result Result = Device.allocateCommandBuffers (&cmdAllocateInfo, &cmd);

    {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Allocate Command Buffer", Result);
    }

    if (Result != vk::Result::eSuccess)
      return false;
    else
      return true;
}
