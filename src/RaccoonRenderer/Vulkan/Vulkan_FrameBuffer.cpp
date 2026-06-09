#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

#if PATACHE_LINUX_OR_UNIX
extern std::uint8_t scaleInt;
#endif

#define PATACHE_ERROR_TEXT_SIZE           64
#define PATACHE_ERROR_TEXT_SIZE_EXTRANULL 65

#include "Vulkan_FrameBuffer.hpp"

bool
Patache::CreateFrameBuffer (Patache::VulkanBackend & rVulkan)
{
  rVulkan.pSwapchainFrameBuffers = static_cast<VkFramebuffer *> (
      std::calloc (rVulkan.swapchainImageCount, sizeof (VkFramebuffer)));

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
#if PATACHE_LINUX_OR_UNIX
        .width  = rVulkan.swapchainExtent.width * scaleInt,
        .height = rVulkan.swapchainExtent.height * scaleInt,
#else
        .width  = rVulkan.swapchainExtent.width,
        .height = rVulkan.swapchainExtent.height,
#endif
        .layers = 1U,
      };

      result = vkCreateFramebuffer (rVulkan.device, &frameBufferInfo, nullptr,
                                    &rVulkan.pSwapchainFrameBuffers[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE_EXTRANULL]{};

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE,
                         "vkCreateFramebuffer() Frame Buffer #%.3u", i + 1);

          Patache::VulkanCheck (errorText, result);

          return false;
        }
    }

  return true;
}
