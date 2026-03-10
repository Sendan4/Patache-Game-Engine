#include "Vulkan_FrameBuffer.hpp"

bool
CreateFrameBuffer (Patache::VulkanBackend & rVulkan)
{
  rVulkan.pSwapchainFrameBuffers = static_cast<VkFramebuffer *> (
      std::malloc (sizeof (VkFramebuffer) * rVulkan.swapchainImageCount));

  if (rVulkan.pSwapchainFrameBuffers == nullptr)
    {
      return false;
    }

  VkResult result;

  for (std::uint8_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      const VkFramebufferCreateInfo frameBufferInfo{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .renderPass      = rVulkan.renderPass,
        .attachmentCount = 1U,
        .pAttachments    = &rVulkan.pSwapchainColorImageViews[i],
        .width           = rVulkan.swapchainExtent.width,
        .height          = rVulkan.swapchainExtent.height,
        .layers          = 1U,
      };

      result = vkCreateFramebuffer (rVulkan.device, &frameBufferInfo, nullptr,
                                    &rVulkan.pSwapchainFrameBuffers[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateFramebuffer() Frame Buffer #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }
    }

  return true;
}
