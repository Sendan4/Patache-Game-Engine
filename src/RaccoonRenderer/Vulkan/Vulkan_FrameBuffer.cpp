#include "Vulkan_FrameBuffer.hpp"

bool
Patata::Engine::CreateFrameBuffer (void)
{
  Vulkan.SwapChainFrameBuffer
      = new vk::Framebuffer[Vulkan.SwapChainImageCount];

  vk::Result Result;

  for (uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      vk::FramebufferCreateInfo FrameBufferInfo (
          {},
          Vulkan.RenderPass,                  // renderPass
          1,                                  // attachmentCount
          &Vulkan.SwapChainColorImageView[i], // pAttachments
          Vulkan.SwapChainExtent.width,       // width
          Vulkan.SwapChainExtent.height,      // height
          1,                                  // layers
          nullptr                             // pNext
      );

      Result = Vulkan.Device.createFramebuffer (
          &FrameBufferInfo, nullptr, &Vulkan.SwapChainFrameBuffer[i]);
      if (Result != vk::Result::eSuccess)
        {
          uint8_t tmp = i;

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patata::Log::VulkanCheck,
              std::string ("Frame Buffer " + std::to_string (++tmp) + '/'
                           + std::to_string (Vulkan.SwapChainImageCount)),
              Result);

          return false;
        }
    }
  return true;
}
