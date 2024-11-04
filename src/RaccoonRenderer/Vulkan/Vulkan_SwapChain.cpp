#include "Vulkan_SwapChain.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateSwapChain (
    std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> & SwapChainInfo)
{
  // Search Presentation modes
  vk::PresentModeKHR * PresentModes;
  uint32_t PresentModesCount = 0;

  vk::Result Result = PhysicalDevice.getSurfacePresentModesKHR(Surface, &PresentModesCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Get Surface Present Modes - Obtaining the count", Result);
  }

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = PhysicalDevice.getSurfacePresentModesKHR(Surface, &PresentModesCount, PresentModes);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Get Surface Present Modes", Result);
  }

  // Search for window surface formats
  vk::SurfaceFormatKHR * SurfaceFormats;
  uint32_t SurfaceFormatsCount = 0;

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Get Surface Formats - Obtaining the count", Result);
  }

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, SurfaceFormats);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Get Surface Formats", Result);
  }

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  {
    int w = 1, h = 1;
    SDL_Vulkan_GetDrawableSize (*pRaccoonInfo->ppWindow, &w, &h);

    SwapChainExtent.width  = w;
    SwapChainExtent.height = h;

    fast_io::io::println (
        #if defined(_WIN64)
        fast_io::out (),
        #endif
        PATATA_TERM_BOLD,
        PATATA_TERM_COLOR_PATATA,
        "Raccoon Renderer",
        PATATA_TERM_RESET,
        PATATA_TERM_BOLD,
        " : Window Drawable Size : ",
        PATATA_TERM_RESET,
        w, " x ", h);
  }

  /*
  Two presentation modes are discarded depending on the configuration.
  If you use Vsync, the Immediate and Mailbox modes are discarded, and
  the available presentation modes that wait for a screen update before
  presenting are sought.

  Mailbox and FIFO are modes that have priority.

  Finding a Present Mode
  */
  vk::PresentModeKHR SelectedPresentMode;
  bool Found = false;

  if (!pRaccoonInfo->pConfiguration->Vsync)
    {
      // No Vsync
      for (uint8_t i = 0; i < PresentModesCount; ++i)
        {
            if (PresentModes[i] == vk::PresentModeKHR::eMailbox) {
                SelectedPresentMode = vk::PresentModeKHR::eMailbox;
                Found = true;
                break;
            }
            else if (PresentModes[i] == vk::PresentModeKHR::eImmediate) {
                SelectedPresentMode = vk::PresentModeKHR::eImmediate;
                Found = true;
                break;
            }
        }
    }
  else {
      // Vsync
      for (uint8_t i = 0; i < PresentModesCount; ++i)
        {
            if (PresentModes[i] == vk::PresentModeKHR::eFifo) {
                SelectedPresentMode = vk::PresentModeKHR::eFifo;
                Found = true;
                break;
            }
            else if (PresentModes[i] == vk::PresentModeKHR::eFifoRelaxed) {
                SelectedPresentMode = vk::PresentModeKHR::eFifoRelaxed;
                Found = true;
                break;
            }
        }
  }

  if (!Found) {
      std::future<void> Err = std::async(
          std::launch::async,
          Patata::Log::FatalErrorMessage,
          "Patata - Raccoon Renderer",
          "No presentation modes found",
          *pRaccoonInfo->pConfiguration);

      return false;
  }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (uint32_t i = 0; i < SurfaceFormatsCount; ++i) {
	  if (pRaccoonInfo->pConfiguration->BitDepth10) {
		  // 10 Bits
		  if (SurfaceFormats[i].format == vk::Format::eA2R10G10B10UnormPack32
				|| SurfaceFormats[i].format == vk::Format::eA2B10G10R10UnormPack32) {
			  SelectedSurfaceFormat = SurfaceFormats[i];
              Found                 = true;
			  break;
		  }
	  }
	  else {
		  // 8 Bits
		  if (SurfaceFormats[i].format == vk::Format::eR8G8B8A8Unorm
				|| SurfaceFormats[i].format == vk::Format::eB8G8R8A8Unorm) {
			  SelectedSurfaceFormat = SurfaceFormats[i];
              Found                 = true;
			  break;
		  }
	  }
  }

  #if defined(DEBUG)
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainPresentMode = SelectedPresentMode;
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainImageColorFormat = SelectedSurfaceFormat.format;
  pRaccoonInfo->pPatataEngineInfo->VkSwapchainImageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
  #endif

  if (!Found)
    {
        std::future<void> Err = std::async(
            std::launch::async,
            Patata::Log::FatalErrorMessage,
            "Patata - Raccoon Renderer",
            "No surface formats found",
            *pRaccoonInfo->pConfiguration);

      return false;
    }

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  Result = PhysicalDevice.getSurfaceCapabilitiesKHR (Surface, &SurfaceCapabilities);
  {
	  std::future<void> ReturnVulkanCheck = std::async(
			std::launch::async, Patata::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);
  }

  if (Result != vk::Result::eSuccess) {
      std::future<void> ReturnVulkanErr = std::async(std::launch::async,
             Patata::Log::FatalErrorMessage,
             "Patata Engine - Raccoon Renderer",
             "The surface area capabilities of this device could not be obtained.",
             *pRaccoonInfo->pConfiguration);

      return false;
  }

  // Create SwapChain
  vk::SwapchainCreateInfoKHR SwapChainCreateInfo({},
      Surface,                                // surface
      SurfaceCapabilities.minImageCount,      // minImageCount
      SelectedSurfaceFormat.format,           // imageFormat
      vk::ColorSpaceKHR::eSrgbNonlinear,      // imageColorSpace
      SwapChainExtent,                        // imageExtent
      1,                                      // imageArrayLayers
      vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst, // imageUsage
      vk::SharingMode::eExclusive,            // imageSharingMode
      1,                                      // queueFamilyIndexCount
      &GraphicsQueueFamilyIndex,              // pQueueFamilyIndices
      SurfaceCapabilities.currentTransform,   // preTransform
      vk::CompositeAlphaFlagBitsKHR::eOpaque, // compositeAlpha
      SelectedPresentMode,                    // presentMode
      true,                                   // clipped
      OldSwapChain,                           // oldSwapChain
      nullptr                                 // pNext
  );

  Result = Device.createSwapchainKHR (&SwapChainCreateInfo, nullptr, &SwapChain);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "SwapChain", Result);
  }

  delete[] PresentModes;
  delete[] SurfaceFormats;

  if (Result != vk::Result::eSuccess) {
      std::future<void> ReturnVulkanErr = std::async(std::launch::async,
             Patata::Log::FatalErrorMessage,
             "Patata Engine - Raccoon Renderer",
             "Swapchain creation failure",
             *pRaccoonInfo->pConfiguration);

      return false;
  }

  // SwapChain Images
  Result = Device.getSwapchainImagesKHR(SwapChain, &SwapChainImageCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Get SwapChain Images KHR - Obtaining the count", Result);
  }

  SwapChainImages = new vk::Image[SwapChainImageCount];

  Result = Device.getSwapchainImagesKHR(SwapChain, &SwapChainImageCount, SwapChainImages);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
        Patata::Log::VulkanCheck, "Get SwapChain Images KHR", Result);
  }

  std::get<0>(SwapChainInfo) = SelectedPresentMode;
  std::get<1>(SwapChainInfo) = SelectedSurfaceFormat.format;
  std::get<2>(SwapChainInfo) = vk::ColorSpaceKHR::eSrgbNonlinear;

  return true;
}

// Recreate SwapChain
void Patata::Graphics::RaccoonRenderer::VulkanBackend::RecreateSwapChain (SDL_Event & Event)
{
    // Minimization
    vk::SurfaceCapabilitiesKHR Sc;
    vk::Result Result = PhysicalDevice.getSurfaceCapabilitiesKHR (Surface, &Sc);

    if (Result != vk::Result::eSuccess)
      {
      std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
          Patata::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);
      }

     while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
      {
         Result = PhysicalDevice.getSurfaceCapabilitiesKHR (Surface, &Sc);

         if (Result != vk::Result::eSuccess)
           {
             std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
                Patata::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);
           }

         SDL_WaitEvent (&Event);
      }

    // Cambio de tamaño a la swapchain
    Result = Device.waitIdle();

    fast_io::io::println (
        #if defined(_WIN64)
        fast_io::out (),
        #endif
        "\n", PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " : Recreating SwapChain",
        PATATA_TERM_RESET);

    {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
		Patata::Log::VulkanCheck, "Device Wait Idle", Result);
    }

    if (Result != vk::Result::eSuccess) {
        std::future<void> ReturnVulkanErr = std::async(std::launch::async,
               Patata::Log::FatalErrorMessage,
               "Patata Engine - Raccoon Renderer",
               "Could not wait for the device",
               *pRaccoonInfo->pConfiguration);

        return;
    }

    // Destroy SwapChain Related Resources

    Device.destroySemaphore (AcquireSemaphore);
    Device.destroySemaphore (SubmitSemaphore);

    Device.destroyRenderPass (RenderPass);

    for (uint8_t i = 0; i < SwapChainImageCount; ++i)
      {
        Device.destroyFramebuffer (SwapChainFrameBuffer[i]);
        Device.destroyImageView (SwapChainColorImageView[i]);
      }

    Device.freeCommandBuffers (CommandPool, 1, &cmd);
    Device.destroyCommandPool(CommandPool);

    OldSwapChain = SwapChain;

    // Create Swapchain And Resources

    std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> SwapChainInfo;
    CreateSwapChain (SwapChainInfo);
    // the new swapchain is ready, I don't need the old swapchain.
    Device.destroySwapchainKHR (OldSwapChain);

    if (!CreateImageView(SwapChainInfo)) {
        std::future<void> Err = std::async(std::launch::async,
               Patata::Log::FatalErrorMessage,
               "Patata Engine - Raccoon Renderer",
               "Color image view recreation failed",
               *pRaccoonInfo->pConfiguration);

        return;
    }

    if (!CreateRenderPass(SwapChainInfo)) {
        std::future<void> Err = std::async(std::launch::async,
               Patata::Log::FatalErrorMessage,
               "Patata Engine - Raccoon Renderer",
               "Failed to recreate the rendering pass",
               *pRaccoonInfo->pConfiguration);

        return;
    }

    if (!CreateFrameBuffer()) {
        std::future<void> Err = std::async(std::launch::async,
               Patata::Log::FatalErrorMessage,
               "Patata Engine - Raccoon Renderer",
               "Frame buffer recreation failed",
               *pRaccoonInfo->pConfiguration);

        return;
    }

    if(!CreateCommandPool()) {
        std::future<void> Err = std::async(std::launch::async,
               Patata::Log::FatalErrorMessage,
               "Patata Engine - Raccoon Renderer",
               "Command pool recreation failed",
               *pRaccoonInfo->pConfiguration);

        return;
    }

    if (!CreateCommandBuffer ())
      {
          std::future<void> Err = std::async(std::launch::async,
                 Patata::Log::FatalErrorMessage,
                 "Patata Engine - Raccoon Renderer",
                 "Command buffer reallocation failed",
                 *pRaccoonInfo->pConfiguration);

        return;
      }

    if (!CreateSemaphores ())
      {
          std::future<void> Err = std::async(std::launch::async,
                 Patata::Log::FatalErrorMessage,
                 "Patata Engine - Raccoon Renderer",
                 "Semaphore recreation failed",
                 *pRaccoonInfo->pConfiguration);

        return;
    }

    // Imgui
    #if defined(DEBUG)
    ImGui_ImplVulkan_SetMinImageCount(Sc.minImageCount);
    #endif
}
