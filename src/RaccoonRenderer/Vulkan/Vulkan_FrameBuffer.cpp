#include "Vulkan_FrameBuffer.hpp"

bool
CreateFrameBuffer (Patache::VulkanBackend & rVulkan)
{
  if (rVulkan.pSwapchainFrameBuffers == nullptr)
    rVulkan.pSwapchainFrameBuffers = new vk::Framebuffer[rVulkan.swapchainImageCount];

  vk::Result result;

  for (std::uint8_t i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      const vk::FramebufferCreateInfo frameBufferInfo{
        .renderPass      = rVulkan.renderPass,
        .attachmentCount = 1,
        .pAttachments    = &rVulkan.pSwapchainColorImageViews[i],
        .width           = rVulkan.swapchainExtent.width,
        .height          = rVulkan.swapchainExtent.height,
        .layers          = 1,
      };

      result = rVulkan.device.createFramebuffer (&frameBufferInfo, nullptr,
                                                 &rVulkan.pSwapchainFrameBuffers[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "Frame Buffer #%.3u", i + 1);

          std::future<void> returnVulkanCheck
              = std::async (std::launch::async, Patache::VulkanCheck, errorText, result);

          return false;
        }
    }
  return true;
}
