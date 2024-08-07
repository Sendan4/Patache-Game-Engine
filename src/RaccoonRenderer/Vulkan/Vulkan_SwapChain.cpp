#include "Vulkan_SwapChain.hpp"

std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateSwapChain (
    uint32_t & GraphicsQueueFamilyIndex, YAML::Node CONFIG, SDL_Window * WINDOW)
{
  // Search Presentation modes
  vk::PresentModeKHR * PresentModes;
  uint32_t PresentModesCount = 0;

  vk::Result Result = PhysicalDevice.getSurfacePresentModesKHR(Surface, &PresentModesCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck0 = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Present Modes - Obtaining the count", Result);
  }

  PresentModes = new vk::PresentModeKHR[PresentModesCount];

  Result = PhysicalDevice.getSurfacePresentModesKHR(Surface, &PresentModesCount, PresentModes);
  {
	  std::future<void> ReturnVulkanCheck1 = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Present Modes", Result);
  }

  // Search for window surface formats
  vk::SurfaceFormatKHR * SurfaceFormats;
  uint32_t SurfaceFormatsCount = 0;

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck2 = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Formats - Obtaining the count", Result);
  }

  SurfaceFormats = new vk::SurfaceFormatKHR[SurfaceFormatsCount];

  Result = PhysicalDevice.getSurfaceFormatsKHR(Surface, &SurfaceFormatsCount, SurfaceFormats);
  {
	  std::future<void> ReturnVulkanCheck3 = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get Surface Formats", Result);
  }

  /*
  Obtain the size of the drawable surface at this time.
  Extent2D
  */
  {
	  int w = 1, h = 1;
	  SDL_Vulkan_GetDrawableSize(WINDOW, &w, &h);

	  SwapChainExtent.width  = w;
	  SwapChainExtent.height = h;
  }

  /*
  Two presentation modes are discarded depending on the configuration.
  If you use Vsync, the Immediate and Mailbox modes are discarded, and
  the available presentation modes that wait for a screen update before
  presenting are sought.

  Mailbox and FIFO are modes that have priority.
  */
  vk::PresentModeKHR SelectedPresentMode;
  bool Found = false;

  if (!CONFIG["patata-engine"]["raccoon-renderer"]["vsync"].as<bool> ())
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
      Patata::Log::FatalErrorMessage("Patata - Raccoon Renderer", "No presentation modes found", CONFIG);
      return {vk::PresentModeKHR::eImmediate, vk::Format::eUndefined, vk::ColorSpaceKHR::eSrgbNonlinear};
  }

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;
  Found = false;

  for (uint32_t i = 0; i < SurfaceFormatsCount; ++i) {
	  if (CONFIG["patata-engine"]["raccoon-renderer"]["10bit-depth"].as<bool> ()) {
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
                                      "No surface formats found", CONFIG);
      return { vk::PresentModeKHR::eImmediate, vk::Format::eUndefined,
               vk::ColorSpaceKHR::eSrgbNonlinear };
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
  SwapChainCreateInfo.oldSwapchain          = nullptr;

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

  return { SelectedPresentMode, SelectedSurfaceFormat.format, vk::ColorSpaceKHR::eSrgbNonlinear };
}
