#include "Vulkan_FrameBuffer.hpp"

bool
Patata::Engine::CreateFrameBuffer (void)
{
  if (Vulkan.SwapChainFrameBuffer == VK_NULL_HANDLE)
    Vulkan.SwapChainFrameBuffer
        = new vk::Framebuffer[Vulkan.SwapChainImageCount];

  vk::Result Result;

  for (std::uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      vk::FramebufferCreateInfo FrameBufferInfo{
        .renderPass      = Vulkan.RenderPass,
        .attachmentCount = 1,
        .pAttachments    = &Vulkan.SwapChainColorImageView[i],
        .width           = Vulkan.SwapChainExtent.width,
        .height          = Vulkan.SwapChainExtent.height,
        .layers          = 1,
      };

      Result = Vulkan.Device.createFramebuffer (
          &FrameBufferInfo, nullptr, &Vulkan.SwapChainFrameBuffer[i]);

      if (Result != vk::Result::eSuccess)
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "Frame Buffer #%.3u", i + 1);

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patata::Log::VulkanCheck, ErrorText, Result);

          return false;
        }
    }
  return true;
}
