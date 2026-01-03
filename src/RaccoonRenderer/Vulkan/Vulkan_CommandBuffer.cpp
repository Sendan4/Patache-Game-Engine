#include "Vulkan_CommandBuffer.hpp"

bool
CreateCommandBuffer (Patache::VulkanBackend & rVulkan)
{
  vk::CommandBufferAllocateInfo cmdAllocateInfo[2]{ {
                                                        .level = vk::CommandBufferLevel::ePrimary,
                                                        .commandBufferCount = 1,
                                                    },
                                                    {
                                                        .level = vk::CommandBufferLevel::eSecondary,
                                                        .commandBufferCount = 1,
                                                    } };

  if (rVulkan.pCmd == nullptr)
    {
      rVulkan.pCmd = static_cast<vk::CommandBuffer *> (
          std::malloc (sizeof (vk::CommandBuffer) * rVulkan.swapchainImageCount));
    }

  if (rVulkan.pSubmitCmd == nullptr)
    {
      rVulkan.pSubmitCmd = static_cast<vk::CommandBuffer *> (
          std::malloc (sizeof (vk::CommandBuffer) * rVulkan.swapchainImageCount));
    }

  vk::Result result;

  for (std::uint_fast32_t i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      // Render command buffer
      cmdAllocateInfo[0].commandPool = rVulkan.pCommandPools[i];
      result = rVulkan.device.allocateCommandBuffers (&cmdAllocateInfo[0], &rVulkan.pCmd[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "Allocate Command Buffer #%.3lu",
                         i);

          std::future<void> returnVulkanCheck
              = std::async (std::launch::async, Patache::VulkanCheck, errorText, result);

          return false;
        }

      // Submit command buffer
      cmdAllocateInfo[1].commandPool = rVulkan.pCommandPools[i];
      result = rVulkan.device.allocateCommandBuffers (&cmdAllocateInfo[1], &rVulkan.pSubmitCmd[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "Allocate Submit Command Buffer #%.3lu", i);

          std::future<void> returnVulkanCheck
              = std::async (std::launch::async, Patache::VulkanCheck, errorText, result);

          return false;
        }
    }

  return true;
}
