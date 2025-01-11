#include "Vulkan_SwapChain.hpp"

bool
Patata::RaccoonRenderer::CreateSwapChain (
    Patata::SwapChainInfo & SwapChainInfo)
{
  // Search Presentation modes
  uint32_t             PresentModesCount = 0;
  vk::PresentModeKHR * PresentModes      = nullptr;

  vk::Result Result = Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Vulkan.Surface, &PresentModesCount, nullptr);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck,
          "Get Surface Present Modes - Obtaining the count", Result);
    }

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = Vulkan.PhysicalDevice.getSurfacePresentModesKHR (
      Vulkan.Surface, &PresentModesCount, PresentModes);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Present Modes", Result);
    }

  // Search for window surface formats
  uint32_t               SurfaceFormatsCount = 0;
  vk::SurfaceFormatKHR * SurfaceFormats      = nullptr;

  Result = Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Vulkan.Surface, &SurfaceFormatsCount, nullptr);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Formats - Obtaining the count", Result);
    }

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = Vulkan.PhysicalDevice.getSurfaceFormatsKHR (
      Vulkan.Surface, &SurfaceFormatsCount, SurfaceFormats);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Formats", Result);
    }

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  std::future<void> SDL_Vulkan_GetDrawableSize_Async
      = std::async (std::launch::async, [this] (void) {
          int w = 1, h = 1;
          SDL_Vulkan_GetDrawableSize (*pRaccoonInfo->ppWindow, &w, &h);

          Vulkan.SwapChainExtent.width  = w;
          Vulkan.SwapChainExtent.height = h;

          fast_io::io::println (
#if defined(_WIN64)
              fast_io::out (),
#endif
              PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
              PATATA_TERM_RESET, PATATA_TERM_BOLD,
              " : Window Drawable Size : ", PATATA_TERM_RESET, w, " x ", h);
        });

  /*
  Two presentation modes are discarded depending on the configuration.
  If you use Vsync, the Immediate and Mailbox modes are discarded, and
  the available presentation modes that wait for a screen update before
  presenting are sought.

  Mailbox and FIFO are modes that have priority.

  Finding a Present Mode
  */
  vk::PresentModeKHR SelectedPresentMode;
  bool               Found = false;

  if (!pRaccoonInfo->pConfiguration->Vsync)
    {
      // No Vsync
      for (uint8_t i = 0; i < PresentModesCount; ++i)
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
      for (uint8_t i = 0; i < PresentModesCount; ++i)
        {
          if (PresentModes[i] == vk::PresentModeKHR::eFifo)
            {
              SelectedPresentMode = vk::PresentModeKHR::eFifo;
              Found               = true;
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
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata - Raccoon Renderer", "No presentation modes found",
          *pRaccoonInfo->pConfiguration);

      return false;
    }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (uint32_t i = 0; i < SurfaceFormatsCount; ++i)
    {
      if (pRaccoonInfo->pConfiguration->BitDepth10)
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
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata - Raccoon Renderer", "No surface formats found",
          *pRaccoonInfo->pConfiguration);

      return false;
    }

#if PATATA_DEBUG == 1
  // saving the data for display in imgui
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainPresentMode
      = SelectedPresentMode;
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainImageColorFormat
      = SelectedSurfaceFormat.format;
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainImageColorSpace
      = vk::ColorSpaceKHR::eSrgbNonlinear;
#endif

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Vulkan.Surface, &SurfaceCapabilities);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Capabilities KHR", Result);
    }

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patata::Log::FatalErrorMessage,
                        "Patata Engine - Raccoon Renderer",
                        "The surface area capabilities of this device could "
                        "not be obtained.",
                        *pRaccoonInfo->pConfiguration);

      return false;
    }

  SDL_Vulkan_GetDrawableSize_Async.wait ();

  // Create SwapChain
  vk::SwapchainCreateInfoKHR SwapChainCreateInfo (
      {},
      Vulkan.Surface,                    // surface
      SurfaceCapabilities.minImageCount, // minImageCount
      SelectedSurfaceFormat.format,      // imageFormat
      vk::ColorSpaceKHR::eSrgbNonlinear, // imageColorSpace
      Vulkan.SwapChainExtent,            // imageExtent
      1,                                 // imageArrayLayers
      vk::ImageUsageFlagBits::eColorAttachment
          | vk::ImageUsageFlagBits::eTransferDst, // imageUsage
      vk::SharingMode::eExclusive,                // imageSharingMode
      1,                                          // queueFamilyIndexCount
      &Vulkan.GraphicsQueueFamilyIndex,           // pQueueFamilyIndices
      SurfaceCapabilities.currentTransform,       // preTransform
      vk::CompositeAlphaFlagBitsKHR::eOpaque,     // compositeAlpha
      SelectedPresentMode,                        // presentMode
      true,                                       // clipped
      Vulkan.OldSwapChain,                        // oldSwapChain
      nullptr                                     // pNext
  );

  Result = Vulkan.Device.createSwapchainKHR (&SwapChainCreateInfo, nullptr,
                                             &Vulkan.SwapChain);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "SwapChain", Result);
    }

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer", "Swapchain creation failure",
          *pRaccoonInfo->pConfiguration);

      return false;
    }

  // SwapChain Images
  Result = Vulkan.Device.getSwapchainImagesKHR (
      Vulkan.SwapChain, &Vulkan.SwapChainImageCount, nullptr);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck,
          "Get SwapChain Images KHR - Obtaining the count", Result);
    }

  Vulkan.SwapChainImages = new vk::Image[Vulkan.SwapChainImageCount];

  Result = Vulkan.Device.getSwapchainImagesKHR (
      Vulkan.SwapChain, &Vulkan.SwapChainImageCount, Vulkan.SwapChainImages);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get SwapChain Images KHR", Result);
    }

  SwapChainInfo.PresentMode      = SelectedPresentMode;
  SwapChainInfo.ImageColorFormat = SelectedSurfaceFormat.format;
  SwapChainInfo.ImageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;

  return true;
}

// Recreate SwapChain
void
Patata::RaccoonRenderer::RecreateSwapChain (SDL_Event & Event)
{
  // Minimization
  vk::SurfaceCapabilitiesKHR Sc;
  vk::Result                 Result
      = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (Vulkan.Surface, &Sc);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Capabilities KHR", Result);
    }

  while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
    {
      Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (Vulkan.Surface,
                                                                &Sc);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patata::Log::VulkanCheck,
                            "Get Surface Capabilities KHR", Result);
        }

      SDL_WaitEvent (&Event);
    }

  // Begin Resize SwapChain
  Result = Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Device Wait Idle", Result);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer", "Could not wait for the device",
          *pRaccoonInfo->pConfiguration);

      return;
    }

  // Destroy SwapChain Related Resources
  Vulkan.Device.destroySemaphore (Vulkan.AcquireSemaphore);
  Vulkan.Device.destroySemaphore (Vulkan.SubmitSemaphore);

  for (uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroyFramebuffer (Vulkan.SwapChainFrameBuffer[i]);
      Vulkan.Device.destroyImageView (Vulkan.SwapChainColorImageView[i]);
    }

  Vulkan.cmd.reset (vk::CommandBufferResetFlagBits::eReleaseResources);

  Vulkan.OldSwapChain = Vulkan.SwapChain;

  Patata::SwapChainInfo SwapChainInfo;
  CreateSwapChain (SwapChainInfo);

  // the new swapchain is ready, I don't need the old swapchain.
  Vulkan.Device.destroySwapchainKHR (Vulkan.OldSwapChain);
  Vulkan.OldSwapChain = nullptr;

  if (!CreateImageView (SwapChainInfo))
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer",
          "Color image view recreation failed", *pRaccoonInfo->pConfiguration);

      return;
    }

  std::future<bool> CreateCommandBuffer_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateCommandBuffer, this);

  std::future<bool> CreateFrameBuffer_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateFrameBuffer, this);

  std::future<bool> CreateSemaphores_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateSemaphores, this);

  CreateCommandBuffer_Async.wait ();
  if (!CreateCommandBuffer_Async.get ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer",
          "Command buffer recreation failed", *pRaccoonInfo->pConfiguration);

      return;
    }

  CreateFrameBuffer_Async.wait ();
  if (!CreateFrameBuffer_Async.get ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer", "Frame buffer recreation failed",
          *pRaccoonInfo->pConfiguration);

      return;
    }

  CreateSemaphores_Async.wait ();
  if (!CreateSemaphores_Async.get ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer", "Semaphore recreation failed",
          *pRaccoonInfo->pConfiguration);

      return;
    }

    // Imgui
#if PATATA_DEBUG == 1
  ImGui_ImplVulkan_SetMinImageCount (Sc.minImageCount);
#endif
}
