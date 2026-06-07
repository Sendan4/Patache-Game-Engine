#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

#define PATACHE_ERROR_TEXT_SIZE           70
#define PATACHE_ERROR_TEXT_SIZE_EXTRANULL 71

#include "Vulkan_CommandBuffer.hpp"

bool
Patache::CreateCommandBuffer (Patache::VulkanBackend & rVulkan)
{
  VkCommandBufferAllocateInfo cmdAllocateInfo{ .sType
                                               = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                               .pNext       = nullptr,
                                               .commandPool = VK_NULL_HANDLE,
                                               .level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                               .commandBufferCount = 1U };

  rVulkan.pCmd = static_cast<VkCommandBuffer *> (
      std::calloc (rVulkan.swapchainImageCount, sizeof (VkCommandBuffer)));

  if (rVulkan.pCmd == nullptr)
    {
      return false;
    }

  VkResult result;

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      // Render command buffer
      cmdAllocateInfo.commandPool = rVulkan.pCommandPools[i];

      result = vkAllocateCommandBuffers (rVulkan.device, &cmdAllocateInfo, &rVulkan.pCmd[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkAllocateCommandBuffers() Primary Render Command Buffer #%.3u", i);

          Patache::VulkanCheck (errorText, result);

          return false;
        }
    }

  return true;
}
