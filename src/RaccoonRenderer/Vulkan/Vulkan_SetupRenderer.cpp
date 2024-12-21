#include "Vulkan_SetupRenderer.hpp"

bool
Patata::RaccoonRenderer::InitVulkanBackend (void)
{
  if (!CreateInstance ())
    return false;

  // Search a Physical Device
  if (!SelectDevice ())
    return false;

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  Vulkan.GraphicsQueueFamilyIndex = CreateLogicalDeviceAndCreateQueue ();

#if defined(DEBUG)
  std::future<bool> CreateImguiDescriptorPool_Async
      = std::async (std::launch::async,
                    &Patata::RaccoonRenderer::CreateImguiDescriptorPool, this);
  std::future<bool> CreateImguiPipelineCache_Async
      = std::async (std::launch::async,
                    &Patata::RaccoonRenderer::CreateImguiPipelineCache, this);
#endif

  // Create a surface for the window to draw on
  if (!SDL_Vulkan_CreateSurface (
          *pRaccoonInfo->ppWindow, Vulkan.Instance,
          reinterpret_cast<VkSurfaceKHR *> (&Vulkan.Surface)))
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_RESET, PATATA_TERM_BOLD,
          "SDL Create Window Surface : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Fail", PATATA_TERM_RESET);

      Patata::Log::FatalErrorMessage ("SDL", SDL_GetError (),
                                      *pRaccoonInfo->pConfiguration);
      return false;
    }

  Patata::SwapChainInfo SwapChainInfo;
  if (!CreateSwapChain (SwapChainInfo))
    return false;

  if (!CreateImageView (SwapChainInfo))
    return false;

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance.

   std::future<bool> CreateDepthBuffer_Async = std::async(std::launch::async,
        [this](void) -> bool {
            return CreateDepthBuffer ();
        });
    */

  if (!CreateRenderPass (SwapChainInfo))
    return false;

  if (!CreateCommandPool ())
    return false;

  std::future<bool> CreateFrameBuffer_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateFrameBuffer, this);

  std::future<bool> CreateCommandBuffer_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateCommandBuffer, this);

  std::future<bool> CreateFence_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateFence, this);

  std::future<bool> CreateSemaphores_Async = std::async (
      std::launch::async, &Patata::RaccoonRenderer::CreateSemaphores, this);

  if (!CreatePipeline ())
    return false;

  std::future<void> VulkanInfo_Async
      = std::async (std::launch::async, &Patata::RaccoonRenderer::VulkanInfo,
                    this, SwapChainInfo);

#if defined(DEBUG)
  CreateImguiPipelineCache_Async.wait ();
  if (!CreateImguiPipelineCache_Async.get ())
    return false;
  CreateImguiDescriptorPool_Async.wait ();
  if (!CreateImguiDescriptorPool_Async.get ())
    return false;
  InitImguiVulkan (*pRaccoonInfo->ppWindow);
#endif

  /*
  CreateDepthBuffer_Async.wait();
  if (!CreateDepthBuffer_Async.get()) return;
  */
  CreateFrameBuffer_Async.wait ();
  if (!CreateFrameBuffer_Async.get ())
    return false;
  CreateCommandBuffer_Async.wait ();
  if (!CreateCommandBuffer_Async.get ())
    return false;
  CreateFence_Async.wait ();
  if (!CreateFence_Async.get ())
    return false;
  CreateSemaphores_Async.wait ();
  if (!CreateSemaphores_Async.get ())
    return false;
  VulkanInfo_Async.wait ();

  return true;
}

void
Patata::RaccoonRenderer::CloseVulkanBackend (void)
{
  vk::Result Result = Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Device Wait Idle", Result);
    }

  Result = Vulkan.Queue.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Queue Wait Idle", Result);
    }

  // Color
  for (uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroyFramebuffer (Vulkan.SwapChainFrameBuffer[i]);
      Vulkan.Device.destroyImageView (Vulkan.SwapChainColorImageView[i]);
    }

  Vulkan.SwapChainFrameBuffer    = nullptr;
  Vulkan.SwapChainColorImageView = nullptr;

  Vulkan.Device.destroySwapchainKHR (Vulkan.SwapChain);
  delete[] Vulkan.SwapChainImages;
  Vulkan.SwapChainImages = nullptr;

// Imgui
#if defined(DEBUG)
  ImGui_ImplVulkan_Shutdown ();
  Vulkan.Device.destroyDescriptorPool (Vulkan.ImguiDescriptorPool);
  Vulkan.Device.destroyPipelineCache (Vulkan.ImguiPipelineCache);
#endif

  Vulkan.Device.destroyRenderPass (Vulkan.RenderPass);

  // Primitivas de sincronizacion
  Vulkan.Device.destroySemaphore (Vulkan.AcquireSemaphore);
  Vulkan.Device.destroySemaphore (Vulkan.SubmitSemaphore);

  Vulkan.Device.destroyFence (Vulkan.Fence);

  Vulkan.Device.destroyCommandPool (Vulkan.CommandPool);

  Vulkan.Device.destroy ();

  Vulkan.Instance.destroySurfaceKHR (Vulkan.Surface);

  Vulkan.Instance.destroy ();
}
