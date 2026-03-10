#include "Vulkan_SincronizationPrimitives.hpp"

bool
CreateSemaphores (Patache::VulkanBackend & rVulkan)
{
  static constexpr VkSemaphoreCreateInfo semaphoreInfo{
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0U
  };

  VkResult result;

  rVulkan.pImageAvailableSemaphores = static_cast<VkSemaphore *> (
      std::malloc (sizeof (VkSemaphore) * rVulkan.swapchainImageCount));

  if (rVulkan.pImageAvailableSemaphores == nullptr)
    {
      return false;
    }

  rVulkan.pImageFinishedSemaphores = static_cast<VkSemaphore *> (
      std::malloc (sizeof (VkSemaphore) * rVulkan.swapchainImageCount));

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
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateSemaphore() Image Available Semaphore #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }

      // ImageFinishedSemaphore
      result = vkCreateSemaphore (rVulkan.device, &semaphoreInfo, nullptr,
                                  &rVulkan.pImageFinishedSemaphores[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateSemaphore() Image Finished Semaphore #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }
    }

  return true;
}

bool
CreateFence (Patache::VulkanBackend & rVulkan)
{
  static constexpr VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = VK_FENCE_CREATE_SIGNALED_BIT };

  VkResult result;

  rVulkan.pInFlightFences
      = static_cast<VkFence *> (std::malloc (sizeof (VkFence) * rVulkan.swapchainImageCount));

  if (rVulkan.pInFlightFences == nullptr)
    {
      return false;
    }

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      result = vkCreateFence (rVulkan.device, &fenceInfo, nullptr, &rVulkan.pInFlightFences[i]);

      if (result != VK_SUCCESS)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateFence() In Flight Fence #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }
    }

  return true;
}
