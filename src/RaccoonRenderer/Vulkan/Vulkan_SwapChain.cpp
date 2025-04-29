#include "Vulkan_SwapChain.hpp"

bool
Patache::Engine::CreateSwapChain (Patache::SwapChainInfo & SwapChainInfo)
{
  // Search Presentation modes
  std::uint32_t        PresentModesCount = 0;
  vk::PresentModeKHR * PresentModes      = nullptr;

  vk::Result Result = Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Vulkan.Surface, &PresentModesCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get Surface Present Modes - Obtaining the count", Result);

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Vulkan.Surface, &PresentModesCount, PresentModes);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get Surface Present Modes", Result);

  // Search for window surface formats
  uint32_t               SurfaceFormatsCount = 0;
  vk::SurfaceFormatKHR * SurfaceFormats      = nullptr;

  Result = Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Vulkan.Surface, &SurfaceFormatsCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get Surface Formats - Obtaining the count", Result);

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Vulkan.Surface, &SurfaceFormatsCount, SurfaceFormats);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get Surface Formats", Result);

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  std::future<void> SDL_Vulkan_GetDrawableSize_Async
      = std::async (std::launch::async, [this] (void) {
          int w = 1, h = 1;
          SDL_GetWindowSizeInPixels (GameWindow, &w, &h);

          Vulkan.SwapChainExtent.width  = w;
          Vulkan.SwapChainExtent.height = h;
        });

  /*
  Two presentation modes are discarded depending on the configuration.
  If you use Vsync, the Immediate and Mailbox modes are discarded, and
  the available presentation modes that wait for a screen update before
  presenting are sought.

  Mailbox and FIFO are modes that have priority.

  Finding a Present Mode
  */
  vk::PresentModeKHR SelectedPresentMode = vk::PresentModeKHR::eFifo;
  bool               Found               = false;

  if (!configuration.Vsync)
    {
      // No Vsync
      for (std::uint8_t i = 0; i < PresentModesCount; ++i)
        {
          if (PresentModes[i] == vk::PresentModeKHR::eMailbox)
            {
              SelectedPresentMode = vk::PresentModeKHR::eMailbox;
              Found               = true;
              break;
            }
          else if (PresentModes[i] == vk::PresentModeKHR::eImmediate)
            {
              SelectedPresentMode = vk::PresentModeKHR::eImmediate;
              Found               = true;
              break;
            }
        }
    }
  else
    {
      // Vsync
      for (std::uint8_t i = 0; i < PresentModesCount; ++i)
        {
          if (PresentModes[i] == vk::PresentModeKHR::eFifo)
            {
              Found = true;
              break;
            }
          else if (PresentModes[i] == vk::PresentModeKHR::eFifoRelaxed)
            {
              SelectedPresentMode = vk::PresentModeKHR::eFifoRelaxed;
              Found               = true;
              break;
            }
        }
    }

  delete[] PresentModes;

  if (!Found)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache engine - Raccoon Renderer",
                        "No presentation modes found", configuration);

      return false;
    }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (std::uint32_t i = 0; i < SurfaceFormatsCount; ++i)
    {
      if (configuration.BitDepth10)
        {
          // 10 Bits
          if (SurfaceFormats[i].format == vk::Format::eA2R10G10B10UnormPack32
              || SurfaceFormats[i].format
                     == vk::Format::eA2B10G10R10UnormPack32)
            {
              SelectedSurfaceFormat = SurfaceFormats[i];
              Found                 = true;
              break;
            }
        }
      else
        {
          // 8 Bits
          if (SurfaceFormats[i].format == vk::Format::eR8G8B8A8Unorm
              || SurfaceFormats[i].format == vk::Format::eB8G8R8A8Unorm)
            {
              SelectedSurfaceFormat = SurfaceFormats[i];
              Found                 = true;
              break;
            }
        }
    }

  delete[] SurfaceFormats;

  if (!Found)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache engine - Raccoon Renderer",
                        "No surface formats found", configuration);

      return false;
    }

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Vulkan.Surface, &SurfaceCapabilities);
  //

#if PATACHE_DEBUG == 1
  // saving the data for display in imgui
  engineInfo.VkSwapchainPresentMode      = SelectedPresentMode;
  engineInfo.VkSwapchainImageColorFormat = SelectedSurfaceFormat.format;
  engineInfo.VkSwapchainImageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;
  engineInfo.VkMinImageCount             = SurfaceCapabilities.minImageCount;
#endif

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Get Surface Capabilities KHR", Result);

      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "The surface area capabilities of this device could "
                        "not be obtained.",
                        configuration);

      return false;
    }

  SDL_Vulkan_GetDrawableSize_Async.wait ();

  // Create SwapChain
  vk::SwapchainCreateInfoKHR SwapChainCreateInfo{
    .surface = Vulkan.Surface,
    .minImageCount
    = SurfaceCapabilities.minImageCount + configuration.AddImageCount,
    .imageFormat      = SelectedSurfaceFormat.format,
    .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
    .imageExtent      = Vulkan.SwapChainExtent,
    .imageArrayLayers = 1,
    .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment
                  | vk::ImageUsageFlagBits::eTransferDst,
    .imageSharingMode      = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices   = &Vulkan.GraphicsQueueFamilyIndex,
    .preTransform          = SurfaceCapabilities.currentTransform,
    .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode           = SelectedPresentMode,
    .clipped               = VK_TRUE,
    .oldSwapchain          = Vulkan.OldSwapChain
  };

  Result = Vulkan.Device.createSwapchainKHR (&SwapChainCreateInfo, nullptr,
                                             &Vulkan.SwapChain);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, "SwapChain", Result);

      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "Swapchain creation failure", configuration);

      return false;
    }

  // SwapChain Images
  Result = Vulkan.Device.getSwapchainImagesKHR (
      Vulkan.SwapChain, &Vulkan.SwapChainImageCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get SwapChain Images KHR - Obtaining the count", Result);

  Vulkan.SwapChainImages = new vk::Image[Vulkan.SwapChainImageCount];

  Result = Vulkan.Device.getSwapchainImagesKHR (
      Vulkan.SwapChain, &Vulkan.SwapChainImageCount, Vulkan.SwapChainImages);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get SwapChain Images KHR", Result);

  SwapChainInfo.PresentMode      = SelectedPresentMode;
  SwapChainInfo.ImageColorFormat = SelectedSurfaceFormat.format;
  SwapChainInfo.ImageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;

  return true;
}

// Recreate SwapChain
void
Patache::Engine::RecreateSwapChain (SDL_Event & Event)
{
  // Minimization
  vk::SurfaceCapabilitiesKHR Sc;
  vk::Result                 Result
      = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (Vulkan.Surface, &Sc);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Get Surface Capabilities KHR", Result);

      return;
    }

  while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
    {
      Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (Vulkan.Surface,
                                                                &Sc);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Get Surface Capabilities KHR", Result);

          return;
        }

      SDL_WaitEvent (&Event);
    }

  // Begin Resize SwapChain
  Result = Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Device Wait Idle", Result);

      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "Could not wait for the device", configuration);

      return;
    }

  std::future<void> DestroyObjects_Async = std::async (
      std::launch::async, [this] (void) {
        for (uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
          {
            Vulkan.Device.destroyFramebuffer (Vulkan.SwapChainFrameBuffer[i]);
            Vulkan.Device.destroyImageView (Vulkan.SwapChainColorImageView[i]);
            Vulkan.Device.destroySemaphore (Vulkan.ImageAvailableSemaphore[i]);
            Vulkan.Device.destroySemaphore (Vulkan.ImageFinishedSemaphore[i]);
          }
      });

  /*Vulkan.Device.resetCommandPool (
      Vulkan.CommandPool, vk::CommandPoolResetFlagBits::eReleaseResources);*/

  Vulkan.OldSwapChain = Vulkan.SwapChain;

  Patache::SwapChainInfo SwapChainInfo;
  CreateSwapChain (SwapChainInfo);

  // the new swapchain is ready, I don't need the old swapchain.
  Vulkan.Device.destroySwapchainKHR (Vulkan.OldSwapChain);
  Vulkan.OldSwapChain = VK_NULL_HANDLE;

  DestroyObjects_Async.wait ();

  if (!CreateImageView (SwapChainInfo))
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "Color image view recreation failed", configuration);

      return;
    }

  std::future<bool> CreateFrameBuffer_Async = std::async (
      std::launch::async, &Patache::Engine::CreateFrameBuffer, this);

  std::future<bool> CreateSemaphores_Async = std::async (
      std::launch::async, &Patache::Engine::CreateSemaphores, this);

  CreateFrameBuffer_Async.wait ();
  if (!CreateFrameBuffer_Async.get ())
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "Frame buffer recreation failed", configuration);

      return;
    }

  CreateSemaphores_Async.wait ();
  if (!CreateSemaphores_Async.get ())
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
                        "Semaphore recreation failed", configuration);

      return;
    }
}
