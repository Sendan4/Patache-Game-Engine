#include "Vulkan_Swapchain.hpp"

bool
CreateSwapchain (Patache::Engine * const pEngine, Patache::SwapchainInfo & rSwapchainInfo)
{
  // Search Presentation modes
  std::uint32_t presentModesCount{ 0U };

  VkResult result{ vkGetPhysicalDeviceSurfacePresentModesKHR (
      pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &presentModesCount, nullptr) };

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck,
          "vkGetPhysicalDeviceSurfacePresentModesKHR() Obtaining the count", result) };
    }

  VkPresentModeKHR * pPresentModes{ static_cast<VkPresentModeKHR *> (
      std::malloc (sizeof (VkPresentModeKHR) * presentModesCount)) };

  if (pPresentModes == nullptr)
    {
      return false;
    }

  result = vkGetPhysicalDeviceSurfacePresentModesKHR (
      pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &presentModesCount, pPresentModes);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkGetPhysicalDeviceSurfacePresentModesKHR()",
          result) };
    }

  // Search for window surface formats
  std::uint32_t surfaceFormatsCount{ 0U };

  result = vkGetPhysicalDeviceSurfaceFormatsKHR (
      pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &surfaceFormatsCount, nullptr);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck,
          "vkGetPhysicalDeviceSurfaceFormatsKHR() Obtaining the count", result) };
    }

  VkSurfaceFormatKHR * pSurfaceFormats{ static_cast<VkSurfaceFormatKHR *> (
      std::malloc (sizeof (VkSurfaceFormatKHR) * surfaceFormatsCount)) };

  if (pSurfaceFormats == nullptr)
    {
      return false;
    }

  result = vkGetPhysicalDeviceSurfaceFormatsKHR (pEngine->vulkan.physicalDevice,
                                                 pEngine->vulkan.surface, &surfaceFormatsCount,
                                                 pSurfaceFormats);

  if (result != VK_SUCCESS)
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
          int w{ 1 }, h{ 1 };
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
  VkPresentModeKHR selectedPresentMode{ VK_PRESENT_MODE_FIFO_KHR };
  bool             found{ false };

  if (!pEngine->configuration.vsync)
    {
      // No Vsync
      for (std::uint8_t i{ 0U }; i < presentModesCount; ++i)
        {
          if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
              selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
              found               = true;
              break;
            }
          else if (pPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
              selectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
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
          if (pPresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
            {
              found = true;
              break;
            }
          else if (pPresentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            {
              selectedPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
              found               = true;
              break;
            }
        }
    }

  if (pPresentModes != nullptr)
    {
      std::free (pPresentModes);
      pPresentModes = nullptr;
    }

  if (!found)
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "No presentation modes found", std::cref (pEngine->configuration)) };

      return false;
    }

  // Finding a surface format.
  VkSurfaceFormatKHR selectedSurfaceFormat{};
  found = false;

  for (std::uint32_t i{ 0U }; i < surfaceFormatsCount; ++i)
    {
      if (pEngine->configuration.bitDepth10)
        {
          // 10 Bits
          if (pSurfaceFormats[i].format == VK_FORMAT_A2B10G10R10_UNORM_PACK32
              || pSurfaceFormats[i].format == VK_FORMAT_A2R10G10B10_UNORM_PACK32)
            {
              selectedSurfaceFormat = pSurfaceFormats[i];
              found                 = true;
              break;
            }
        }
      else
        {
          // 8 Bits
          if (pSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM
              || pSurfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
            {
              selectedSurfaceFormat = pSurfaceFormats[i];
              found                 = true;
              break;
            }
        }
    }

  if (pSurfaceFormats == nullptr)
    {
      std::free (pSurfaceFormats);
      pSurfaceFormats = nullptr;
    }

  if (!found)
    {
      std::future<void> err{ std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
          "No surface formats found", std::cref (pEngine->configuration)) };

      return false;
    }

  VkSurfaceCapabilitiesKHR surfaceCapabilities{};

  result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (
      pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &surfaceCapabilities);

  if (result != VK_SUCCESS)
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

#if PATACHE_DEBUG == 1 && PATACHE_LINUX_OR_UNIX
  ImGuiIO & rIO{ ImGui::GetIO () };

  rIO.FontGlobalScale = scaleInt;
#endif

  // Create SwapChain
  const VkSwapchainCreateInfoKHR swapchainCreateInfo{
    .sType           = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext           = nullptr,
    .flags           = 0U,
    .surface         = pEngine->vulkan.surface,
    .minImageCount   = (surfaceCapabilities.minImageCount + pEngine->configuration.addImageCount),
    .imageFormat     = selectedSurfaceFormat.format,
    .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
#if PATACHE_LINUX_OR_UNIX
    .imageExtent{ .width  = pEngine->vulkan.swapchainExtent.width * scaleInt,
                  .height = pEngine->vulkan.swapchainExtent.height * scaleInt },
#else
    .imageExtent{ .width  = pEngine->vulkan.swapchainExtent.width,
                  .height = pEngine->vulkan.swapchainExtent.height },
#endif
    .imageArrayLayers      = 1U,
    .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1U,
    .pQueueFamilyIndices   = &pEngine->vulkan.graphicsQueueFamilyIndex,
    .preTransform          = surfaceCapabilities.currentTransform,
    .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode           = selectedPresentMode,
    .clipped               = VK_TRUE,
    .oldSwapchain          = pEngine->vulkan.oldSwapchain
  };

  result = vkCreateSwapchainKHR (pEngine->vulkan.device, &swapchainCreateInfo, nullptr,
                                 &pEngine->vulkan.swapchain);

  if (result != VK_SUCCESS)
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
          rSwapchainInfo.imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }) };

  // Swapchain Images
  result = vkGetSwapchainImagesKHR (pEngine->vulkan.device, pEngine->vulkan.swapchain,
                                    &pEngine->vulkan.swapchainImageCount, nullptr);

  if (result != VK_SUCCESS)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkGetSwapchainImagesKHR() Obtaining the count",
          result) };
    }

  // TODO: no destruir la memoria de las imagenes rapidamente
  if (pEngine->vulkan.pSwapchainImages != nullptr)
    {
      std::free (pEngine->vulkan.pSwapchainImages);
    }

  pEngine->vulkan.pSwapchainImages = static_cast<VkImage *> (
      std::malloc (sizeof (VkImage) * pEngine->vulkan.swapchainImageCount));

  if (pEngine->vulkan.pSwapchainImages == nullptr)
    {
      return false;
    }

  result = vkGetSwapchainImagesKHR (pEngine->vulkan.device, pEngine->vulkan.swapchain,
                                    &pEngine->vulkan.swapchainImageCount,
                                    pEngine->vulkan.pSwapchainImages);

  if (result != VK_SUCCESS)
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
  VkResult result{ vkDeviceWaitIdle (pEngine->vulkan.device) };

  if (result != VK_SUCCESS)
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
              vkDestroyFramebuffer (pEngine->vulkan.device,
                                    pEngine->vulkan.pSwapchainFrameBuffers[i], nullptr);
              vkDestroyImageView (pEngine->vulkan.device,
                                  pEngine->vulkan.pSwapchainColorImageViews[i], nullptr);
              vkDestroySemaphore (pEngine->vulkan.device,
                                  pEngine->vulkan.pImageAvailableSemaphores[i], nullptr);
              vkDestroySemaphore (pEngine->vulkan.device,
                                  pEngine->vulkan.pImageFinishedSemaphores[i], nullptr);
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

  vkDestroySwapchainKHR (pEngine->vulkan.device, pEngine->vulkan.oldSwapchain, nullptr);
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
