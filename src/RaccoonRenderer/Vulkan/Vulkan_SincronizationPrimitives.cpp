#include "Vulkan_SincronizationPrimitives.hpp"

bool
CreateSemaphores (Patache::VulkanBackend & Vulkan)
{
  static constexpr const vk::SemaphoreCreateInfo SemaphoreInfo{};

  vk::Result Result;

  if (Vulkan.ImageAvailableSemaphore == VK_NULL_HANDLE)
    Vulkan.ImageAvailableSemaphore
        = new vk::Semaphore[Vulkan.SwapChainImageCount];

  if (Vulkan.ImageFinishedSemaphore == VK_NULL_HANDLE)
    Vulkan.ImageFinishedSemaphore
        = new vk::Semaphore[Vulkan.SwapChainImageCount];

  for (std::uint32_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      // ImageAvailableSemaphore
      Result = Vulkan.Device.createSemaphore (
          &SemaphoreInfo, nullptr, &Vulkan.ImageAvailableSemaphore[i]);

      if (Result != vk::Result::eSuccess)
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "Image Available Semaphore #%.3u", i + 1);

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patache::Log::VulkanCheck,
              ErrorText, Result);

          return false;
        }

      // ImageFinishedSemaphore
      Result = Vulkan.Device.createSemaphore (
          &SemaphoreInfo, nullptr, &Vulkan.ImageFinishedSemaphore[i]);

      if (Result != vk::Result::eSuccess)
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "Image Finished Semaphore #%.3u", i + 1);

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patache::Log::VulkanCheck,
              ErrorText, Result);

          return false;
        }
    }

  return true;
}

bool
CreateFence (Patache::VulkanBackend & Vulkan)
{
  static constexpr vk::FenceCreateInfo FenceInfo{
    .flags = vk::FenceCreateFlagBits::eSignaled
  };

  vk::Result Result;

  if (Vulkan.InFlightFences == VK_NULL_HANDLE)
    Vulkan.InFlightFences = new vk::Fence[Vulkan.SwapChainImageCount];

  for (std::uint32_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Result = Vulkan.Device.createFence (&FenceInfo, nullptr,
                                          &Vulkan.InFlightFences[i]);

      if (Result != vk::Result::eSuccess)
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                         "In Flight Fence #%.3u", i + 1);

          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patache::Log::VulkanCheck,
              ErrorText, Result);

          return false;
        }
    }

  return true;
}
