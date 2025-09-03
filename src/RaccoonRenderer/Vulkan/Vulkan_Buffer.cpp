#include "Vulkan_Buffer.hpp"

bool
Patache::Engine::BeginCopyBuffer (void)
{
  const vk::CommandBufferAllocateInfo cmdTransferInfo{
    .commandPool = Vulkan.CommandPool, .commandBufferCount = 1
  };

  vk::Result Result = Vulkan.Device.allocateCommandBuffers (
      &cmdTransferInfo, &Vulkan.CmdTransfer);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Transfer Command Buffer", Result);

      return false;
    }

  const vk::CommandBufferBeginInfo cmdTransferBeginInfo{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  Result = Vulkan.CmdTransfer.begin (&cmdTransferBeginInfo);
  assert ("Fail to start recording commands for transfer"
          && (Result == vk::Result::eSuccess));

  return true;
}

void
Patache::Engine::CopyTriangle (Patache::Triangle & Triangle)
{
  // Map memory

  // Copy buffer CPU -> GPU
  const vk::BufferCopy copyMemoryRegion{ .srcOffset = 0,
                                         .dstOffset = Triangle.offset,
                                         .size      = sizeof (
                                             Patache::Triangle::vertex) };

  for (std::uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.CmdTransfer.copyBuffer (VK_NULL_HANDLE, Vulkan.renderBuffer[i], 1,
                                     &copyMemoryRegion);

      const vk::BufferMemoryBarrier syncBufferWrite{
        .srcAccessMask       = vk::AccessFlagBits::eTransferWrite,
        .dstAccessMask       = vk::AccessFlagBits::eTransferWrite,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = VK_NULL_HANDLE, // buffer src
        .size                = sizeof (Patache::Triangle::vertex)
      };

      Vulkan.CmdTransfer.pipelineBarrier (vk::PipelineStageFlagBits::eTransfer,
                                          vk::PipelineStageFlagBits::eTransfer,
                                          {}, 0, nullptr, 1, &syncBufferWrite,
                                          0, nullptr);
    }
}

void
Patache::Engine::EndCopyBuffer (void)
{
  [[maybe_unused]] vk::Result Result = Vulkan.CmdTransfer.end ();

  assert ("Fail to end recording commands for transfer"
          && (Result == vk::Result::eSuccess));

  const vk::SubmitInfo submitInfo{ .commandBufferCount = 1,
                                   .pCommandBuffers    = &Vulkan.CmdTransfer };

  Result = Vulkan.Queue.submit (1, &submitInfo, VK_NULL_HANDLE);

  assert ("Fail queue submit for transfer"
          && (Result == vk::Result::eSuccess));

  Result = Vulkan.Queue.waitIdle ();

  assert ("Fail to wait for transfer queue"
          && (Result == vk::Result::eSuccess));

  Vulkan.Device.freeCommandBuffers (Vulkan.CommandPool, 1,
                                    &Vulkan.CmdTransfer);

  Vulkan.CmdTransfer = VK_NULL_HANDLE;
}
