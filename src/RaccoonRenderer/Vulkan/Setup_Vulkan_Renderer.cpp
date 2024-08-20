#include "Setup_Vulkan_Renderer.hpp"

Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanBackend (
    Patata::Config & Config, SDL_Window * Window, bool & WindowResized)
{
  pWindow = Window;
  pWindowResized = &WindowResized;
  pConfiguration = &Config;

  if (!CreateInstance ())
    return;

  // Search a Physical Device
  if (!SelectDevice ())
    return;

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  GraphicsQueueFamilyIndex = CreateLogicalDeviceAndCreateQueue ();

  fast_io::io::print (
#if defined(_WIN64)
      fast_io::out (),
#endif
      PATATA_TERM_RESET,
      PATATA_TERM_BOLD,
      "SDL Create Window Surface : ", PATATA_TERM_RESET);

  // Create a surface for the window to draw on
  if (SDL_Vulkan_CreateSurface (pWindow, Instance,
                                reinterpret_cast<VkSurfaceKHR *> (&Surface)))
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_GREEN, "Success", PATATA_TERM_RESET);
    }
  else
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_YELLOW, "Fail", PATATA_TERM_RESET);

      Patata::Log::FatalErrorMessage ("SDL", SDL_GetError (), *pConfiguration);
      return;
    }

  std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> SwapChainInfo;
  if (!CreateSwapChain (SwapChainInfo)) return;

  if (!CreateImageView(SwapChainInfo)) return;

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D performance.
  */
  if (!CreateDepthBuffer ())
    return;

  if (!CreateRenderPass (SwapChainInfo))
    return;

  if (!CreateFrameBuffer()) return;

  vk::Result Result;

  // Command Pool
  {
    vk::CommandPoolCreateInfo CommandPoolInfo {};
    CommandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    CommandPoolInfo.pNext = nullptr;
    CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

    Result = Device.createCommandPool (&CommandPoolInfo,
                                                  nullptr, &CommandPool);
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Command Pool", Result);
    }
  }

  if(!CreateFence()) return;

  if(!CreateSemaphores()) return;

  VulkanInfo (SwapChainInfo);
}

Patata::Graphics::RaccoonRenderer::VulkanBackend::~VulkanBackend (void)
{
  for (uint8_t i = 0; i < SwapChainImageCount; ++i)
    Device.destroyFramebuffer(SwapChainFrameBuffer[i]);

  delete[] SwapChainFrameBuffer;

  //Device.destroyPipelineLayout(PipeLineLayout);
  Device.destroyRenderPass(RenderPass);

  // Color
  for (uint8_t i = 0; i < SwapChainImageCount; ++i)
    Device.destroyImageView(SwapChainColorImageView[i]);

  delete[] SwapChainColorImageView;

  // Depth
  Device.freeMemory(DepthMemory);
  Device.destroyImageView(DepthView);
  Device.destroyImage(DepthImage);

  Device.destroySemaphore (AcquireSemaphore);
  Device.destroySemaphore (SubmitSemaphore);

  #if defined(DEBUG)
  vk::Result Result = Device.waitForFences(1, &Fence, true, 0);
  {
	    std::future<void> ReturnVulkanCheck = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Wait For Fences", Result);
  }
  #else
  Device.waitForFences(1, &Fence, true, 0);
  #endif

  #if defined(DEBUG)
  Result = Device.resetFences(1, &Fence);
  if (Result != vk::Result::eSuccess) {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Reset Fences", Result);
  }
  #else
  Device.resetFences(1, &Fence);
  #endif

  Device.destroyFence (Fence);

  Device.destroyCommandPool (CommandPool);

  delete[] SwapChainImages;
  Device.destroySwapchainKHR (SwapChain);

  Instance.destroySurfaceKHR (Surface);

  Device.destroy ();
  Instance.destroy ();
}
