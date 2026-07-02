#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

namespace Patache
{
static constexpr std::uint8_t sErrorTextSize{ 64U };
static constexpr std::uint8_t sErrorTextSizeExtraNull{ 65U };
}

#include "Vulkan_SincronizationPrimitives.hpp"

bool
Patache::CreateSemaphores (Patache::VulkanBackend & rVulkan)
{
  static constexpr VkSemaphoreCreateInfo semaphoreInfo{
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0U
  };

  VkResult result;

  rVulkan.pImageAvailableSemaphores = static_cast<VkSemaphore *> (
      std::calloc (rVulkan.swapchainImageCount, sizeof (VkSemaphore)));

  if (rVulkan.pImageAvailableSemaphores == nullptr)
    {
      return false;
    }

  rVulkan.pImageFinishedSemaphores = static_cast<VkSemaphore *> (
      std::calloc (rVulkan.swapchainImageCount, sizeof (VkSemaphore)));

  if (rVulkan.pImageFinishedSemaphores == nullptr)
    {
      return false;
    }

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      // ImageAvailableSemaphore
      result = vkCreateSemaphore (rVulkan.device, &semaphoreInfo, nullptr,
                                  &rVulkan.pImageAvailableSemaphores[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[Patache::sErrorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::sErrorTextSize,
                         "vkCreateSemaphore() Image Available Semaphore #%.3u", i + 1);

          Patache::VulkanCheck (errorText, result);

          return false;
        }

      // ImageFinishedSemaphore
      result = vkCreateSemaphore (rVulkan.device, &semaphoreInfo, nullptr,
                                  &rVulkan.pImageFinishedSemaphores[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[Patache::sErrorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::sErrorTextSize,
                         "vkCreateSemaphore() Image Finished Semaphore #%.3u", i + 1);

          Patache::VulkanCheck (errorText, result);

          return false;
        }
    }

  return true;
}

bool
Patache::CreateFence (Patache::VulkanBackend & rVulkan)
{
  static constexpr VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = VK_FENCE_CREATE_SIGNALED_BIT };

  VkResult result;

  rVulkan.pInFlightFences
      = static_cast<VkFence *> (std::calloc (rVulkan.swapchainImageCount, sizeof (VkFence)));

  if (rVulkan.pInFlightFences == nullptr)
    {
      return false;
    }

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      result = vkCreateFence (rVulkan.device, &fenceInfo, nullptr, &rVulkan.pInFlightFences[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[Patache::sErrorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::sErrorTextSize,
                         "vkCreateFence() In Flight Fence #%.3u", i + 1);

          Patache::VulkanCheck (errorText, result);

          return false;
        }
    }

  return true;
}
