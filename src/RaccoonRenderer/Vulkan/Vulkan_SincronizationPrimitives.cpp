#include "Vulkan_SincronizationPrimitives.hpp"

bool
Patata::RaccoonRenderer::CreateSemaphores (void)
{
  vk::SemaphoreCreateInfo SemaphoreInfo;

  vk::Result Result = Vulkan.Device.createSemaphore (&SemaphoreInfo, nullptr,
                                                     &Vulkan.AcquireSemaphore);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Acquire Semaphore", Result);

      return false;
    }

  Result = Vulkan.Device.createSemaphore (&SemaphoreInfo, nullptr,
                                          &Vulkan.SubmitSemaphore);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Submit Semaphore", Result);

      return false;
    }
  else
    return true;
}

bool
Patata::RaccoonRenderer::CreateFence (void)
{
  vk::FenceCreateInfo FenceInfo (vk::FenceCreateFlagBits::eSignaled, // flags
                                 nullptr                             // pNext
  );

  vk::Result Result
      = Vulkan.Device.createFence (&FenceInfo, nullptr, &Vulkan.Fence);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Fence", Result);

      return false;
    }
  else
    return true;
}
