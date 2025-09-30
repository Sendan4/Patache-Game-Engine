#include "Vulkan_Buffer.hpp"

bool
Patache::Engine::BeginCopyBuffer (void)
{
  const vk::CommandBufferAllocateInfo cmdTransferInfo{ .commandPool        = vulkan.commandPool,
                                                       .commandBufferCount = 1 };

  vk::Result result = vulkan.device.allocateCommandBuffers (&cmdTransferInfo, &vulkan.cmdTransfer);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck = std::async (std::launch::async, Patache::VulkanCheck,
                                                        "Transfer Command Buffer", result);

      return false;
    }

  const vk::CommandBufferBeginInfo cmdTransferBeginInfo{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  result = vulkan.cmdTransfer.begin (&cmdTransferBeginInfo);
  assert ("Fail to start recording commands for transfer" && (result == vk::Result::eSuccess));

  return true;
}

void
Patache::Engine::CopyTriangle (Patache::Triangle & rTriangle)
{
  // Map memory

  // Copy buffer CPU -> GPU
  const vk::BufferCopy copyMemoryRegion{ .srcOffset = 0,
                                         .dstOffset = rTriangle.offset,
                                         .size      = sizeof (Patache::Triangle::vertex) };

  for (std::uint8_t i = 0; i < vulkan.swapchainImageCount; ++i)
    {
      vulkan.cmdTransfer.copyBuffer (VK_NULL_HANDLE, vulkan.pRenderBuffer[i], 1, &copyMemoryRegion);

      const vk::BufferMemoryBarrier syncBufferWrite{ .srcAccessMask
                                                     = vk::AccessFlagBits::eTransferWrite,
                                                     .dstAccessMask
                                                     = vk::AccessFlagBits::eTransferWrite,
                                                     .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                     .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                     .buffer = VK_NULL_HANDLE, // buffer src
                                                     .size   = sizeof (Patache::Triangle::vertex) };

      vulkan.cmdTransfer.pipelineBarrier (vk::PipelineStageFlagBits::eTransfer,
                                          vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 1,
                                          &syncBufferWrite, 0, nullptr);
    }
}

void
Patache::Engine::EndCopyBuffer (void)
{
  [[maybe_unused]] vk::Result result = vulkan.cmdTransfer.end ();

  assert ("Fail to end recording commands for transfer" && (result == vk::Result::eSuccess));

  const vk::SubmitInfo submitInfo{ .commandBufferCount = 1,
                                   .pCommandBuffers    = &vulkan.cmdTransfer };

  result = vulkan.queue.submit (1, &submitInfo, VK_NULL_HANDLE);

  assert ("Fail queue submit for transfer" && (result == vk::Result::eSuccess));

  result = vulkan.queue.waitIdle ();

  assert ("Fail to wait for transfer queue" && (result == vk::Result::eSuccess));

  vulkan.device.freeCommandBuffers (vulkan.commandPool, 1, &vulkan.cmdTransfer);

  vulkan.cmdTransfer = VK_NULL_HANDLE;
}
