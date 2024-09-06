#include "Vulkan_CommandPoolAndBuffer.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateCommandPool (void) {
    vk::CommandPoolCreateInfo CommandPoolInfo (
        vk::CommandPoolCreateFlagBits::eTransient             // flags
        | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, // flags
        GraphicsQueueFamilyIndex,                             // queueFamilyIndex
        nullptr                                               // pNext
    );

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
    vk::CommandBufferAllocateInfo cmdAllocateInfo (
        CommandPool,                      // commandPool
        vk::CommandBufferLevel::ePrimary, // level
        1,                                // commandBufferCount
        nullptr                           // pNext
    );

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
