#include "Vulkan_SincronizationPrimitives.hpp"

bool
CreateSemaphores (Patache::VulkanBackend & rVulkan)
{
  static constexpr vk::SemaphoreCreateInfo semaphoreInfo{};

  vk::Result result;

  if (rVulkan.pImageAvailableSemaphores == nullptr)
    rVulkan.pImageAvailableSemaphores = new vk::Semaphore[rVulkan.swapchainImageCount];

  if (rVulkan.pImageFinishedSemaphores == nullptr)
    rVulkan.pImageFinishedSemaphores = new vk::Semaphore[rVulkan.swapchainImageCount];

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      // ImageAvailableSemaphore
      result = rVulkan.device.createSemaphore (&semaphoreInfo, nullptr,
                                               &rVulkan.pImageAvailableSemaphores[i]);

      if (result != vk::Result::eSuccess)
        {
          char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "vkCreateSemaphore() Image Available Semaphore #%.3u", i + 1);

          std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                           errorText, result) };

          return false;
        }

      // ImageFinishedSemaphore
      result = rVulkan.device.createSemaphore (&semaphoreInfo, nullptr,
                                               &rVulkan.pImageFinishedSemaphores[i]);

      if (result != vk::Result::eSuccess)
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
  constexpr vk::FenceCreateInfo fenceInfo{ .flags = vk::FenceCreateFlagBits::eSignaled };

  vk::Result result;

  if (rVulkan.pInFlightFences == nullptr)
    rVulkan.pInFlightFences = new vk::Fence[rVulkan.swapchainImageCount];

  for (std::uint32_t i{ 0U }; i < rVulkan.swapchainImageCount; ++i)
    {
      result = rVulkan.device.createFence (&fenceInfo, nullptr, &rVulkan.pInFlightFences[i]);

      if (result != vk::Result::eSuccess)
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
