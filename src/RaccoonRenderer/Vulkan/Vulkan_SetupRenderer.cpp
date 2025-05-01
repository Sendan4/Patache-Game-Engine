#include "Vulkan_SetupRenderer.hpp"

struct Vertex
{
  vec2 pos[3]{ { 0.0f, 0.0f }, { 0.5f, 0.5f }, { -0.5f, 0.5 } };
  vec3 color[3]{ { 1.0f, 0.0f, 0.0f },
                 { 0.0f, 1.0f, 0.0f },
                 { 0.0f, 0.0f, 1.0f } };
};

bool
Patache::Engine::RaccoonRendererInit (const Patache::EngineCreateInfo & Info)
{
#if PATACHE_DEBUG == 1
  std::future<void> InitImgui_Async
      = std::async (std::launch::async, &Patache::Engine::InitImgui, this);
#endif

  if (!CreateInstance (Info))
    return false;

  // Search a Physical Device
  if (!SelectDevice ())
    return false;

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  Vulkan.GraphicsQueueFamilyIndex = CreateLogicalDeviceAndCreateQueue ();

#if PATACHE_DEBUG == 1
  InitImgui_Async.wait ();

  std::future<bool> CreateImguiDescriptorPool_Async = std::async (
      std::launch::async, &Patache::Engine::CreateImguiDescriptorPool, this);
  std::future<bool> CreateImguiPipelineCache_Async = std::async (
      std::launch::async, &Patache::Engine::CreateImguiPipelineCache, this);
#endif

  // Create a surface for the window to draw on
  if (!SDL_Vulkan_CreateSurface (
          GameWindow, Vulkan.Instance, nullptr,
          reinterpret_cast<VkSurfaceKHR *> (&Vulkan.Surface)))
    {
      fast_io::io::println (
          PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
          "SDL Create Window Surface : ", PATACHE_TERM_RESET,
          PATACHE_TERM_COLOR_YELLOW, "Fail", PATACHE_TERM_RESET);

      std::future<void> FatalErr
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "SDL", SDL_GetError (), configuration);
      return false;
    }

  Patache::SwapChainInfo SwapChainInfo;
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
      std::launch::async, &Patache::Engine::CreateFrameBuffer, this);

  std::future<bool> CreateCommandBuffer_Async = std::async (
      std::launch::async, &Patache::Engine::CreateCommandBuffer, this);

  std::future<bool> CreateFence_Async
      = std::async (std::launch::async, &Patache::Engine::CreateFence, this);

  std::future<bool> CreateSemaphores_Async = std::async (
      std::launch::async, &Patache::Engine::CreateSemaphores, this);

  std::future<bool> CreatePipeline_Async = std::async (
      std::launch::async, &Patache::Engine::CreatePipeline, this);

  std::future<void> VulkanInfo_Async = std::async (
      std::launch::async, &Patache::Engine::VulkanInfo, this, SwapChainInfo);

  // Create Buffer
  /*std::future<bool> CreateBuffer_Async = std::async (
      std::launch::async, [this] (void) -> bool { return true; });*/

#if PATACHE_DEBUG == 1
  CreateImguiPipelineCache_Async.wait ();
  if (!CreateImguiPipelineCache_Async.get ())
    return false;

  CreateImguiDescriptorPool_Async.wait ();
  if (!CreateImguiDescriptorPool_Async.get ())
    return false;

  InitImguiVulkan ();
#endif

  /*
  CreateDepthBuffer_Async.wait();
  if (!CreateDepthBuffer_Async.get()) return false;
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

  CreatePipeline_Async.wait ();
  if (!CreatePipeline_Async.get ())
    return false;

  /*CreateBuffer_Async.wait ();
  if (!CreateBuffer_Async.get ())
    return false;*/

  return true;
}

void
Patache::Engine::RaccoonRendererClose (void)
{
  if (Vulkan.Device == VK_NULL_HANDLE || Vulkan.Instance == VK_NULL_HANDLE)
    return;

  vk::Result Result = Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Device Wait Idle", Result);
    }

  if (Vulkan.Queue == VK_NULL_HANDLE)
    return;

  Result = Vulkan.Queue.waitIdle ();

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Queue Wait Idle", Result);

  // Color
  for (std::uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroyFramebuffer (Vulkan.SwapChainFrameBuffer[i]);
      Vulkan.Device.destroyImageView (Vulkan.SwapChainColorImageView[i]);
    }

  delete[] Vulkan.SwapChainFrameBuffer;
  delete[] Vulkan.SwapChainColorImageView;
  Vulkan.SwapChainFrameBuffer    = VK_NULL_HANDLE;
  Vulkan.SwapChainColorImageView = VK_NULL_HANDLE;

  Vulkan.Device.destroySwapchainKHR (Vulkan.SwapChain);
  delete[] Vulkan.SwapChainImages;
  Vulkan.SwapChainImages = VK_NULL_HANDLE;

  // Vulkan.Device.destroyBuffer (Vulkan.VertexBuffer);
  // Vulkan.Device.freeMemory (Vulkan.VertexBufferMemory);

// Imgui
#if PATACHE_DEBUG == 1
  ImGui_ImplVulkan_Shutdown ();

  Vulkan.Device.destroyDescriptorPool (Vulkan.ImguiDescriptorPool);
  Vulkan.Device.destroyPipelineCache (Vulkan.ImguiPipelineCache);
#endif

  Vulkan.Device.destroyPipelineCache (Vulkan.GraphicsPipelineCache);
  Vulkan.Device.destroyPipelineLayout (Vulkan.PipelineLayout);
  Vulkan.Device.destroyPipeline (Vulkan.GraphicsPipeline);

  Vulkan.Device.destroyRenderPass (Vulkan.RenderPass);

  // Primitivas de sincronizacion
  for (std::uint32_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroySemaphore (Vulkan.ImageAvailableSemaphore[i]);
      Vulkan.Device.destroySemaphore (Vulkan.ImageFinishedSemaphore[i]);
      Vulkan.Device.destroyFence (Vulkan.InFlightFences[i]);
    }
  delete[] Vulkan.ImageAvailableSemaphore;
  delete[] Vulkan.ImageFinishedSemaphore;
  delete[] Vulkan.InFlightFences;

  Vulkan.ImageAvailableSemaphore = VK_NULL_HANDLE;
  Vulkan.ImageFinishedSemaphore  = VK_NULL_HANDLE;
  Vulkan.InFlightFences          = VK_NULL_HANDLE;

  Vulkan.Device.destroyCommandPool (Vulkan.CommandPool);
  delete[] Vulkan.Cmd;
  Vulkan.Cmd = VK_NULL_HANDLE;

  Vulkan.Device.destroy ();

  Vulkan.Instance.destroySurfaceKHR (Vulkan.Surface);

#if PATACHE_DEBUG == 1
  pfnVkDestroyDebugUtilsMessengerEXT
      = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT> (
          Vulkan.Instance.getProcAddr ("vkDestroyDebugUtilsMessengerEXT"));

  if (pfnVkDestroyDebugUtilsMessengerEXT != nullptr)
    Vulkan.Instance.destroyDebugUtilsMessengerEXT (Vulkan.DebugMessenger);
#endif

  Vulkan.Instance.destroy ();
}
