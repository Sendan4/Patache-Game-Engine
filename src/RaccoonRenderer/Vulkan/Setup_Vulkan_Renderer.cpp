#include "Setup_Vulkan_Renderer.hpp"

Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanBackend (Patata::Graphics::RaccoonRendererCreateInfo * RaccoonInfo)
{
  pRaccoonInfo = RaccoonInfo;

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

  #if defined(DEBUG)
  CreateImguiDescriptorPool();
  CreateImguiPipelineCache();
  #endif

  fast_io::io::print (
    #if defined(_WIN64)
    fast_io::out (),
    #endif
    PATATA_TERM_RESET,
    PATATA_TERM_BOLD,
    "SDL Create Window Surface : ", PATATA_TERM_RESET);

  // Create a surface for the window to draw on
  if (SDL_Vulkan_CreateSurface (*pRaccoonInfo->ppWindow, Instance,
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

      Patata::Log::FatalErrorMessage ("SDL", SDL_GetError (), *pRaccoonInfo->pConfiguration);
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

  if (!CreatePipeline()) return;

  #if defined(DEBUG)
  VulkanImguiSetup (*pRaccoonInfo->ppWindow);
  #endif

  VulkanInfo (SwapChainInfo);
}

Patata::Graphics::RaccoonRenderer::VulkanBackend::~VulkanBackend (void)
{
  vk::Result Result = Device.waitIdle();

  {
      std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
          Patata::Log::VulkanCheck, "Device Wait Idle", Result);
  }

  Result = Queue.waitIdle();

  {
      std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
          Patata::Log::VulkanCheck, "Queue Wait Idle", Result);
  }

  // Imgui
  #if defined(DEBUG)
  ImGui_ImplVulkan_Shutdown();
  Device.destroyDescriptorPool(ImguiDescriptorPool);
  Device.destroyPipelineCache(ImguiPipelineCache);
  #endif

  // Primitivas de sincronizacion
  Device.destroySemaphore (AcquireSemaphore);
  Device.destroySemaphore (SubmitSemaphore);

  Result = Device.waitForFences (1, &Fence, true, std::numeric_limits<uint64_t>::max ());
  {
      std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
          Patata::Log::VulkanCheck, "Wait For Fences", Result);
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
