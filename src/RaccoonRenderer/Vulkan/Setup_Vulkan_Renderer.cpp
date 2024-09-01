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
  // if (!CreateDepthBuffer ()) return;

  if (!CreateRenderPass (SwapChainInfo))
    return;

  if (!CreateFrameBuffer()) return;

  if (!CreateCommandPool()) return;

  if (!CreateCommandBuffer ())
    return;

  if(!CreateFence()) return;

  if(!CreateSemaphores()) return;

  VulkanInfo (SwapChainInfo);
}

Patata::Graphics::RaccoonRenderer::VulkanBackend::~VulkanBackend (void)
{
  // Primitivas de sincronizacion
  Device.destroySemaphore (AcquireSemaphore);
  Device.destroySemaphore (SubmitSemaphore);

  vk::Result Result = Device.waitForFences (1, &Fence, true, std::numeric_limits<uint64_t>::max ());
  {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Wait For Fences", Result);
  }

  Device.destroyFence (Fence);

  //Device.destroyPipelineLayout(PipeLineLayout);
  Device.destroyRenderPass(RenderPass);

  // Color
  for (uint8_t i = 0; i < SwapChainImageCount; ++i)
    {
      Device.destroyFramebuffer (SwapChainFrameBuffer[i]);
      Device.destroyImageView (SwapChainColorImageView[i]);
    }

  delete[] SwapChainFrameBuffer;
  delete[] SwapChainColorImageView;

  // Depth
  //Device.freeMemory(DepthMemory);
  //Device.destroyImageView(DepthView);
  //Device.destroyImage(DepthImage);

  Device.freeCommandBuffers (CommandPool, 1, &cmd);
  Device.destroyCommandPool (CommandPool);

  Device.destroySwapchainKHR (SwapChain);
  delete[] SwapChainImages;

  Instance.destroySurfaceKHR (Surface);

  Device.destroy ();
  Instance.destroy ();
}
