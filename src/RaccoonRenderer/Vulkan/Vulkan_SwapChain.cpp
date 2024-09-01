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
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Present Modes - Obtaining the count", Result);
  }

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = PhysicalDevice.getSurfacePresentModesKHR(Surface, &PresentModesCount, PresentModes);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Present Modes", Result);
  }

  // Search for window surface formats
  vk::SurfaceFormatKHR * SurfaceFormats;
  uint32_t SurfaceFormatsCount = 0;

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Formats - Obtaining the count", Result);
  }

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, SurfaceFormats);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Formats", Result);
  }

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  {
    int w = 1, h = 1;
    SDL_Vulkan_GetDrawableSize(pWindow, &w, &h);

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

  if (!pConfiguration->Vsync)
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
      Patata::Log::FatalErrorMessage("Patata - Raccoon Renderer", "No presentation modes found", *pConfiguration);
      return false;
  }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (uint32_t i = 0; i < SurfaceFormatsCount; ++i) {
	  if (pConfiguration->BitDepth10) {
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

  if (!Found)
    {
      Patata::Log::FatalErrorMessage ("Patata - Raccoon Renderer",
                                      "No surface formats found", *pConfiguration);
      return false;
    }

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities
      = PhysicalDevice.getSurfaceCapabilitiesKHR (Surface);

  // Create SwapChain
  vk::SwapchainCreateInfoKHR SwapChainCreateInfo {};
  SwapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
  SwapChainCreateInfo.pNext = nullptr;
  SwapChainCreateInfo.surface          = Surface;
  SwapChainCreateInfo.minImageCount    = SurfaceCapabilities.minImageCount;
  SwapChainCreateInfo.imageFormat      = SelectedSurfaceFormat.format;
  SwapChainCreateInfo.imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear;
  SwapChainCreateInfo.imageExtent      = SwapChainExtent;
  SwapChainCreateInfo.imageArrayLayers = 1;
  SwapChainCreateInfo.imageUsage   = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  SwapChainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
  SwapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  SwapChainCreateInfo.presentMode    = SelectedPresentMode;
  SwapChainCreateInfo.clipped        = true;
  SwapChainCreateInfo.queueFamilyIndexCount = GraphicsQueueFamilyIndex;
  SwapChainCreateInfo.oldSwapchain          = OldSwapChain;

  Result = Device.createSwapchainKHR (&SwapChainCreateInfo, nullptr, &SwapChain);
  delete[] PresentModes;
  delete[] SurfaceFormats;
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "SwapChain", Result);
  }

  // SwapChain Images
  Result = Device.getSwapchainImagesKHR(SwapChain, &SwapChainImageCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get SwapChain Images KHR - Obtaining the count", Result);
  }

  SwapChainImages = new vk::Image[SwapChainImageCount];
  Result = Device.getSwapchainImagesKHR(SwapChain, &SwapChainImageCount, SwapChainImages);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get SwapChain Images KHR", Result);
  }

  std::get<0>(SwapChainInfo) = SelectedPresentMode;
  std::get<1>(SwapChainInfo) = SelectedSurfaceFormat.format;
  std::get<2>(SwapChainInfo) = vk::ColorSpaceKHR::eSrgbNonlinear;

  return true;
}

// Recreate SwapChain
void Patata::Graphics::RaccoonRenderer::VulkanBackend::RecreateSwapChain (void)
{
    Device.waitIdle();

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
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "color image view recreation failed",
                                        *pConfiguration);
        return;
    }

    if (!CreateRenderPass(SwapChainInfo)) {
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "failed to recreate the rendering pass",
                                        *pConfiguration);
        return;
    }

    if (!CreateFrameBuffer()) {
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "frame buffer recreation failed",
                                        *pConfiguration);
        return;
    }

    if(!CreateCommandPool()) {
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "command pool recreation failed",
                                        *pConfiguration);
        return;
    }

    if (!CreateCommandBuffer ())
      {
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "command buffer reallocation failed",
                                        *pConfiguration);
        return;
      }

    if (!CreateSemaphores ())
      {
        Patata::Log::FatalErrorMessage ("Patata Engine - Raccoon Renderer",
                                        "semaphore recreation failed",
                                        *pConfiguration);
        return;
    }
}
