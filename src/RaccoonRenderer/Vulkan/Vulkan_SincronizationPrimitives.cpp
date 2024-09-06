#include "Vulkan_SincronizationPrimitives.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateSemaphores (void) {
    vk::SemaphoreCreateInfo SemaphoreInfo;

    vk::Result Result = Device.createSemaphore (&SemaphoreInfo, nullptr, &AcquireSemaphore);
    {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Acquire Semaphore", Result);
    }

    if (Result != vk::Result::eSuccess) return false;

    Result = Device.createSemaphore (&SemaphoreInfo, nullptr, &SubmitSemaphore);
    {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Submit Semaphore", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateFence (void) {
    vk::FenceCreateInfo FenceInfo(
        vk::FenceCreateFlagBits::eSignaled, // flags
        nullptr                             // pNext
    );

    vk::Result Result = Device.createFence (&FenceInfo, nullptr, &Fence);
      {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Fence", Result);
      }

    if (Result != vk::Result::eSuccess)
        return false;
    else return true;
}
