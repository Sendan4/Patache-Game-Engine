#include "Vulkan_SwapChain.hpp"

bool
CreateSwapChain (Patache::Engine * const  Engine,
                 Patache::SwapChainInfo & SwapChainInfo)
{
  // Search Presentation modes
  std::uint32_t        PresentModesCount = 0;
  vk::PresentModeKHR * PresentModes      = nullptr;

  vk::Result Result = Engine->Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Engine->Vulkan.Surface, &PresentModesCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get Surface Present Modes - Obtaining the count",
        Result);

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = Engine->Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Engine->Vulkan.Surface, &PresentModesCount, PresentModes);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get Surface Present Modes", Result);

  // Search for window surface formats
  std::uint32_t          SurfaceFormatsCount = 0;
  vk::SurfaceFormatKHR * SurfaceFormats      = nullptr;

  Result = Engine->Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Engine->Vulkan.Surface, &SurfaceFormatsCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get Surface Formats - Obtaining the count",
                      Result);

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = Engine->Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Engine->Vulkan.Surface, &SurfaceFormatsCount, SurfaceFormats);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Get Surface Formats", Result);

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  std::future<void> SDL_Vulkan_GetDrawableSize_Async
      = std::async (std::launch::async, [&Engine] (void) {
          int w = 1, h = 1;
          SDL_GetWindowSizeInPixels (Engine->GameWindow, &w, &h);

          Engine->Vulkan.SwapChainExtent.width  = w;
          Engine->Vulkan.SwapChainExtent.height = h;
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

  if (!Engine->configuration.Vsync)
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
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache engine - Raccoon Renderer",
          "No presentation modes found",
          Engine->configuration);

      return false;
    }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (std::uint32_t i = 0; i < SurfaceFormatsCount; ++i)
    {
      if (Engine->configuration.BitDepth10)
        {
          // 10 Bits
          if (SurfaceFormats[i].format == vk::Format::eA2B10G10R10UnormPack32
              || SurfaceFormats[i].format
                     == vk::Format::eA2R10G10B10UnormPack32)
            {
              SelectedSurfaceFormat = SurfaceFormats[i];
              Found                 = true;
              break;
            }
        }
      else
        {
          // 8 Bits
          if (SurfaceFormats[i].format == vk::Format::eB8G8R8A8Unorm
              || SurfaceFormats[i].format == vk::Format::eR8G8B8A8Unorm)
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
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache engine - Raccoon Renderer",
          "No surface formats found",
          Engine->configuration);

      return false;
    }

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  Result = Engine->Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Engine->Vulkan.Surface, &SurfaceCapabilities);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Get Surface Capabilities KHR", Result);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "The surface area capabilities of this device could "
                     "not be obtained.",
          Engine->configuration);

      return false;
    }

  SDL_Vulkan_GetDrawableSize_Async.wait ();

  // Create SwapChain
  const vk::SwapchainCreateInfoKHR SwapChainCreateInfo{
    .surface = Engine->Vulkan.Surface,
    .minImageCount
    = SurfaceCapabilities.minImageCount + Engine->configuration.AddImageCount,
    .imageFormat      = SelectedSurfaceFormat.format,
    .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
    .imageExtent      = Engine->Vulkan.SwapChainExtent,
    .imageArrayLayers = 1,
    .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment
                  | vk::ImageUsageFlagBits::eTransferDst,
    .imageSharingMode      = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices   = &Engine->Vulkan.GraphicsQueueFamilyIndex,
    .preTransform          = SurfaceCapabilities.currentTransform,
    .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode           = SelectedPresentMode,
    .clipped               = VK_TRUE,
    .oldSwapchain          = Engine->Vulkan.OldSwapChain
  };

  Result = Engine->Vulkan.Device.createSwapchainKHR (
      &SwapChainCreateInfo, nullptr, &Engine->Vulkan.SwapChain);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "SwapChain", Result);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "Swapchain creation failure",
          Engine->configuration);

      return false;
    }

  std::future<void> StoreSwapChainInfo_Async
      = std::async (std::launch::async, [&SwapChainInfo, &SelectedPresentMode,
                                         &SelectedSurfaceFormat] (void) {
          SwapChainInfo.PresentMode      = SelectedPresentMode;
          SwapChainInfo.ImageColorFormat = SelectedSurfaceFormat.format;
          SwapChainInfo.ImageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;
        });

  // SwapChain Images
  Result = Engine->Vulkan.Device.getSwapchainImagesKHR (
      Engine->Vulkan.SwapChain, &Engine->Vulkan.SwapChainImageCount, nullptr);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get SwapChain Images KHR - Obtaining the count",
        Result);

  Engine->Vulkan.SwapChainImages
      = new vk::Image[Engine->Vulkan.SwapChainImageCount];

  Result = Engine->Vulkan.Device.getSwapchainImagesKHR (
      Engine->Vulkan.SwapChain, &Engine->Vulkan.SwapChainImageCount,
      Engine->Vulkan.SwapChainImages);

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck,
        "Get SwapChain Images KHR", Result);

  StoreSwapChainInfo_Async.wait ();

  return true;
}

// Recreate SwapChain
void
RecreateSwapChain (Patache::Engine * const Engine, SDL_Event & Event)
{
  // Minimization
  vk::SurfaceCapabilitiesKHR Sc;
  vk::Result Result = Engine->Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Engine->Vulkan.Surface, &Sc);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Get Surface Capabilities KHR", Result);

      return;
    }

  while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
    {
      Result = Engine->Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
          Engine->Vulkan.Surface, &Sc);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck = std::async (
              std::launch::async, Patache::Log::VulkanCheck,
              "Get Surface Capabilities KHR", Result);

          return;
        }

      SDL_WaitEvent (&Event);
    }

  // Begin Resize SwapChain
  Result = Engine->Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Device Wait Idle", Result);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "Could not wait for the device",
          Engine->configuration);

      return;
    }

  std::future<void> DestroyObjects_Async
      = std::async (std::launch::async, [&Engine] (void) {
          for (std::uint8_t i = 0; i < Engine->Vulkan.SwapChainImageCount; ++i)
            {
              Engine->Vulkan.Device.destroyFramebuffer (
                  Engine->Vulkan.SwapChainFrameBuffer[i]);
              Engine->Vulkan.Device.destroyImageView (
                  Engine->Vulkan.SwapChainColorImageView[i]);
              Engine->Vulkan.Device.destroySemaphore (
                  Engine->Vulkan.ImageAvailableSemaphore[i]);
              Engine->Vulkan.Device.destroySemaphore (
                  Engine->Vulkan.ImageFinishedSemaphore[i]);
            }
        });

  Engine->Vulkan.OldSwapChain = Engine->Vulkan.SwapChain;

  Patache::SwapChainInfo SwapChainInfo;

  CreateSwapChain (Engine, SwapChainInfo);

  DestroyObjects_Async.wait ();

  if (!CreateImageView (Engine->Vulkan, SwapChainInfo))
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "Color image view recreation failed",
          Engine->configuration);

      return;
    }

  std::future<bool> CreateFrameBuffer_Async
      = std::async (std::launch::async, CreateFrameBuffer,
                    std::ref (Engine->Vulkan));

  std::future<bool> CreateSemaphores_Async
      = std::async (std::launch::async, CreateSemaphores,
                    std::ref (Engine->Vulkan));

  Engine->Vulkan.Device.destroySwapchainKHR (Engine->Vulkan.OldSwapChain);
  Engine->Vulkan.OldSwapChain = VK_NULL_HANDLE;

  CreateFrameBuffer_Async.wait ();
  if (!CreateFrameBuffer_Async.get ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "Frame buffer recreation failed",
          Engine->configuration);

      return;
    }

  CreateSemaphores_Async.wait ();
  if (!CreateSemaphores_Async.get ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Raccoon Renderer",
          "Semaphore recreation failed",
          Engine->configuration);

      return;
    }
}
