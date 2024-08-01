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
  Select a desired display mode based on the configuration.

  Mailbox is preferred for a vsync mode, if not available select Fifo

  Immediate mode is used if vsync mode will not be used.
  */
  vk::PresentModeKHR SelectedPresentMode;

  if (CONFIG["patata-engine"]["raccoon-renderer"]["vsync"].as<bool> ())
    {
      for (uint32_t i = 0; i < PresentModesCount; ++i)
        {
          if (PresentModes[i] == vk::PresentModeKHR::eMailbox)
            {
              SelectedPresentMode = vk::PresentModeKHR::eMailbox;
              break;
            }
          if (PresentModes[i] == vk::PresentModeKHR::eFifo)
            {
              SelectedPresentMode = vk::PresentModeKHR::eFifo;
              break;
            }
        }
    }
  else SelectedPresentMode = vk::PresentModeKHR::eImmediate;

  // Finding a surface format.
  vk::SurfaceFormatKHR SelectedSurfaceFormat;

  for (uint32_t i = 0; i < SurfaceFormatsCount; ++i) {
	  if (CONFIG["patata-engine"]["raccoon-renderer"]["10bit-image"].as<bool> ()) {
		  // 10 Bits
		  if (SurfaceFormats[i].format == vk::Format::eA2B10G10R10UnormPack32) {
			  SelectedSurfaceFormat = SurfaceFormats[i];
			  break;
		  }
	  }
	  else {
		  // 8 Bits
		  if (SurfaceFormats[i].format == vk::Format::eB8G8R8A8Srgb ||
				  SurfaceFormats[i].format == vk::Format::eR8G8B8A8Unorm ||
				  SurfaceFormats[i].format == vk::Format::eR8G8B8Srgb) {
			  SelectedSurfaceFormat = SurfaceFormats[i];
			  break;
		  }
	  }
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
  SwapChainCreateInfo.imageColorSpace  = SelectedSurfaceFormat.colorSpace;
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

  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "SwapChain", Result);
  }

  // SwapChain Images
  uint32_t tmpSwapChainImageCount = 0;
  Result = Device.getSwapchainImagesKHR(SwapChain, &tmpSwapChainImageCount, nullptr);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get SwapChain Images KHR - Obtaining the count", Result);
  }

  SwapChainImages = new vk::Image[tmpSwapChainImageCount];
  Result = Device.getSwapchainImagesKHR(SwapChain, &tmpSwapChainImageCount, SwapChainImages);
  {
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Get SwapChain Images KHR", Result);
  }

  SwapChainImageCount = tmpSwapChainImageCount;

  return { SelectedPresentMode, SelectedSurfaceFormat.format, SelectedSurfaceFormat.colorSpace };
}
