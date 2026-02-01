#include "Vulkan_Swapchain.hpp"

bool
CreateSwapchain (Patache::Engine * const pEngine, Patache::SwapchainInfo & rSwapchainInfo)
{
  // Search Presentation modes
  std::uint32_t        presentModesCount{ 0U };
  vk::PresentModeKHR * pPresentModes{ nullptr };

  vk::Result result{ pEngine->vulkan.physicalDevice.getSurfacePresentModesKHR (
      pEngine->vulkan.surface, &presentModesCount, nullptr) };

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck,
          "vkGetPhysicalDeviceSurfacePresentModesKHR() Obtaining the count", result) };
    }

  pPresentModes = new vk::PresentModeKHR[presentModesCount];

  result = pEngine->vulkan.physicalDevice.getSurfacePresentModesKHR (
      pEngine->vulkan.surface, &presentModesCount, pPresentModes);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkGetPhysicalDeviceSurfacePresentModesKHR()",
          result) };
    }

  // Search for window surface formats
  std::uint32_t          surfaceFormatsCount{ 0U };
  vk::SurfaceFormatKHR * pSurfaceFormats{ nullptr };

  result = pEngine->vulkan.physicalDevice.getSurfaceFormatsKHR (pEngine->vulkan.surface,
                                                                &surfaceFormatsCount, nullptr);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck,
          "vkGetPhysicalDeviceSurfaceFormatsKHR() Obtaining the count", result) };
    }

  pSurfaceFormats = new vk::SurfaceFormatKHR[surfaceFormatsCount];

  result = pEngine->vulkan.physicalDevice.getSurfaceFormatsKHR (
      pEngine->vulkan.surface, &surfaceFormatsCount, pSurfaceFormats);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkGetPhysicalDeviceSurfaceFormatsKHR()",
                                                       result) };
    }

    /*
    Obtain the size of the drawable surface at this time.
    Extent2D
    */

#if defined(_WIN64)
  std::future<void> getDrawableSize_Async{ std::async (
      std::launch::async,
      [&pEngine] (void)
        {
          int w = 1, h = 1;
          SDL_GetWindowSizeInPixels (pEngine->pGameWindow, &w, &h);

          pEngine->vulkan.swapchainExtent.width  = w;
          pEngine->vulkan.swapchainExtent.height = h;
        }) };
#endif
  /*
  Two presentation modes are discarded depending on the configuration.
  If you use Vsync, the Immediate and Mailbox modes are discarded, and
  the available presentation modes that wait for a screen update before
  presenting are sought.

  Mailbox and FIFO are modes that have priority.

  Finding a Present Mode
  */
  vk::PresentModeKHR selectedPresentMode{ vk::PresentModeKHR::eFifo };
  bool               found{ false };

  if (!pEngine->configuration.vsync)
    {
      // No Vsync
      for (std::uint8_t i{ 0U }; i < presentModesCount; ++i)
        {
          if (pPresentModes[i] == vk::PresentModeKHR::eMailbox)
            {
              selectedPresentMode = vk::PresentModeKHR::eMailbox;
              found               = true;
              break;
            }
          else if (pPresentModes[i] == vk::PresentModeKHR::eImmediate)
            {
              selectedPresentMode = vk::PresentModeKHR::eImmediate;
              found               = true;
              break;
            }
        }
    }
  else
    {
      // Vsync
      for (std::uint8_t i{ 0U }; i < presentModesCount; ++i)
        {
          if (pPresentModes[i] == vk::PresentModeKHR::eFifo)
            {
              found = true;
              break;
            }
          else if (pPresentModes[i] == vk::PresentModeKHR::eFifoRelaxed)
            {
              selectedPresentMode = vk::PresentModeKHR::eFifoRelaxed;
              found               = true;
              break;
            }
        }
    }

  delete[] pPresentModes;

  if (!found)
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "No presentation modes found", std::cref (pEngine->configuration)) };

      return false;
    }

  // Finding a surface format.
  vk::SurfaceFormatKHR selectedSurfaceFormat;
  found = false;

  for (std::uint32_t i{ 0U }; i < surfaceFormatsCount; ++i)
    {
      if (pEngine->configuration.bitDepth10)
        {
          // 10 Bits
          if (pSurfaceFormats[i].format == vk::Format::eA2B10G10R10UnormPack32
              || pSurfaceFormats[i].format == vk::Format::eA2R10G10B10UnormPack32)
            {
              selectedSurfaceFormat = pSurfaceFormats[i];
              found                 = true;
              break;
            }
        }
      else
        {
          // 8 Bits
          if (pSurfaceFormats[i].format == vk::Format::eB8G8R8A8Unorm
              || pSurfaceFormats[i].format == vk::Format::eR8G8B8A8Unorm)
            {
              selectedSurfaceFormat = pSurfaceFormats[i];
              found                 = true;
              break;
            }
        }
    }

  delete[] pSurfaceFormats;

  if (!found)
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "No surface formats found", std::cref (pEngine->configuration)) };

      return false;
    }

  vk::SurfaceCapabilitiesKHR surfaceCapabilities;

  result = pEngine->vulkan.physicalDevice.getSurfaceCapabilitiesKHR (pEngine->vulkan.surface,
                                                                     &surfaceCapabilities);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR()",
          result) };

      std::future<void> returnVulkanErr{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "The surface area capabilities of this device could "
          "not be obtained.",
          std::cref (pEngine->configuration)) };

      return false;
    }

#if defined(_WIN64)
  getDrawableSize_Async.wait ();
#endif

  // Create SwapChain
  const vk::SwapchainCreateInfoKHR swapchainCreateInfo{
    .surface          = pEngine->vulkan.surface,
    .minImageCount    = (surfaceCapabilities.minImageCount + pEngine->configuration.addImageCount),
    .imageFormat      = selectedSurfaceFormat.format,
    .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
    .imageExtent      = pEngine->vulkan.swapchainExtent,
    .imageArrayLayers = 1U,
    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
    .imageSharingMode      = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount = 1U,
    .pQueueFamilyIndices   = &pEngine->vulkan.graphicsQueueFamilyIndex,
    .preTransform          = surfaceCapabilities.currentTransform,
    .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode           = selectedPresentMode,
    .clipped               = VK_TRUE,
    .oldSwapchain          = pEngine->vulkan.oldSwapchain
  };

  result = pEngine->vulkan.device.createSwapchainKHR (&swapchainCreateInfo, nullptr,
                                                      &pEngine->vulkan.swapchain);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkCreateSwapchainKHR()", result) };

      std::future<void> returnVulkanErr{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Swapchain creation failure", std::cref (pEngine->configuration)) };

      return false;
    }

  std::future<void> storeSwapchainInfo_Async{ std::async (
      std::launch::async,
      [&rSwapchainInfo, &selectedPresentMode, &selectedSurfaceFormat] (void)
        {
          rSwapchainInfo.presentMode      = selectedPresentMode;
          rSwapchainInfo.imageColorFormat = selectedSurfaceFormat.format;
          rSwapchainInfo.imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;
        }) };

  // SwapChain Images
  result = pEngine->vulkan.device.getSwapchainImagesKHR (
      pEngine->vulkan.swapchain, &pEngine->vulkan.swapchainImageCount, nullptr);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkGetSwapchainImagesKHR() Obtaining the count",
          result) };
    }

  if (pEngine->vulkan.pSwapchainImages == nullptr)
    pEngine->vulkan.pSwapchainImages
        = new vk::Image[pEngine->vulkan.swapchainImageCount](VK_NULL_HANDLE);

  result = pEngine->vulkan.device.getSwapchainImagesKHR (pEngine->vulkan.swapchain,
                                                         &pEngine->vulkan.swapchainImageCount,
                                                         pEngine->vulkan.pSwapchainImages);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkGetSwapchainImagesKHR()", result) };
    }

  storeSwapchainInfo_Async.wait ();

  return true;
}

// Recreate SwapChain
void
RecreateSwapchain (Patache::Engine * const pEngine)
{
  vk::Result result{ pEngine->vulkan.device.waitIdle () };

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkDeviceWaitIdle()", result) };

      std::future<void> returnVulkanErr{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Could not wait for the device", std::cref (pEngine->configuration)) };

      return;
    }

  std::future<void> destroyObjects_Async{ std::async (
      std::launch::async,
      [&pEngine] (void)
        {
          for (std::uint8_t i{ 0U }; i < pEngine->vulkan.swapchainImageCount; ++i)
            {
              pEngine->vulkan.device.destroyFramebuffer (pEngine->vulkan.pSwapchainFrameBuffers[i]);
              pEngine->vulkan.device.destroyImageView (
                  pEngine->vulkan.pSwapchainColorImageViews[i]);
              pEngine->vulkan.device.destroySemaphore (
                  pEngine->vulkan.pImageAvailableSemaphores[i]);
              pEngine->vulkan.device.destroySemaphore (pEngine->vulkan.pImageFinishedSemaphores[i]);
            }
        }) };

  pEngine->vulkan.oldSwapchain = pEngine->vulkan.swapchain;

  Patache::SwapchainInfo swapchainInfo;

  if (!CreateSwapchain (pEngine, swapchainInfo))
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Swapchain recreation in resize event failed", std::cref (pEngine->configuration)) };

      return;
    }

  destroyObjects_Async.wait ();

  if (!CreateImageView (pEngine->vulkan, swapchainInfo))
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Color image view recreation failed", std::cref (pEngine->configuration)) };

      return;
    }

  std::future<bool> createFrameBuffer_Async{ std::async (std::launch::async, CreateFrameBuffer,
                                                         std::ref (pEngine->vulkan)) };

  std::future<bool> createSemaphores_Async{ std::async (std::launch::async, CreateSemaphores,
                                                        std::ref (pEngine->vulkan)) };

  pEngine->vulkan.device.destroySwapchainKHR (pEngine->vulkan.oldSwapchain);
  pEngine->vulkan.oldSwapchain = VK_NULL_HANDLE;

  createFrameBuffer_Async.wait ();
  if (!createFrameBuffer_Async.get ())
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Frame buffer recreation failed", std::cref (pEngine->configuration)) };

      return;
    }

  createSemaphores_Async.wait ();
  if (!createSemaphores_Async.get ())
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "Semaphore recreation failed", std::cref (pEngine->configuration)) };

      return;
    }
}
