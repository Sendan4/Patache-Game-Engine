#include "Vulkan_Render.hpp"

bool
Patache::Engine::BeginRender (SDL_Event & Event)
{
  vk::Result Result;

  /*
   * Resizing Window is diferent in Linux with wayland
   * Wayland dont send vk::Result::eErrorOutOfDateKHR
   * The compositor throw the new size of window
   * and advise when you should resize.
   * */
#if defined(__linux__)
  if (ResizingPending && Resize)
    {
      RecreateSwapChain (this);

      ResizingPending = false;
      Resize          = false;

      Vulkan.CurrentFrame = 0;
      Vulkan.ImageIndex   = 0;

      wl_surface_commit (WaylandWindow.Surface);
    }
#endif

  // Wait Fences
  Result = Vulkan.Device.waitForFences (
      1, &Vulkan.InFlightFences[Vulkan.CurrentFrame], VK_TRUE, UINT64_MAX);

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "Wait For Fence #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  // Acquire Next Image
  const vk::AcquireNextImageInfoKHR NextImageInfo{
    .swapchain  = Vulkan.SwapChain,
    .timeout    = UINT64_MAX,
    .semaphore  = Vulkan.ImageAvailableSemaphore[Vulkan.CurrentFrame],
    .fence      = VK_NULL_HANDLE,
    .deviceMask = 1
  };

  Result = Vulkan.Device.acquireNextImage2KHR (&NextImageInfo,
                                               &Vulkan.ImageIndex);

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Acquire Next Image 2 KHR", Result);
#endif

  // Resize
  if (Result == vk::Result::eErrorOutOfDateKHR)
    {
      // Minimization
      vk::SurfaceCapabilitiesKHR Sc;
      vk::Result Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
          Vulkan.Surface, &Sc);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Get Surface Capabilities KHR", Result);

          return false;
        }

      while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
        {
          Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
              Vulkan.Surface, &Sc);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Get Surface Capabilities KHR", Result);

              return false;
            }

          SDL_WaitEvent (&Event);
        }

      RecreateSwapChain (this);
    }

  // Reset Fences
  Result = Vulkan.Device.resetFences (
      1, &Vulkan.InFlightFences[Vulkan.CurrentFrame]);

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "Reset Fence #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  static constexpr vk::CommandBufferBeginInfo cmdBufferBeginInfo{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  // Begin Command Buffer
  Result = Vulkan.Cmd[Vulkan.CurrentFrame].begin (&cmdBufferBeginInfo);

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Command Buffer Begin", Result);
#endif

  // Begin RenderPass
  {
    Vulkan.ClearColor.color.float32[0] = clearColor.r;
    Vulkan.ClearColor.color.float32[1] = clearColor.g;
    Vulkan.ClearColor.color.float32[2] = clearColor.b;
    Vulkan.ClearColor.color.float32[3] = clearColor.a;

    Vulkan.RenderArea.extent   = vk::Extent2D{ Vulkan.SwapChainExtent.width,
                                             Vulkan.SwapChainExtent.height };
    Vulkan.RenderArea.offset.x = 0;
    Vulkan.RenderArea.offset.y = 0;

    const vk::RenderPassBeginInfo Info{
      .renderPass      = Vulkan.RenderPass,
      .framebuffer     = Vulkan.SwapChainFrameBuffer[Vulkan.ImageIndex],
      .renderArea      = Vulkan.RenderArea,
      .clearValueCount = 1,
      .pClearValues    = &Vulkan.ClearColor,
    };

    Vulkan.Cmd[Vulkan.CurrentFrame].beginRenderPass (
        Info, vk::SubpassContents::eInline);
  }

  Vulkan.Cmd[Vulkan.CurrentFrame].bindPipeline (
      vk::PipelineBindPoint::eGraphics, Vulkan.GraphicsPipeline);

  /*vk::DeviceSize offsets[] = { 0 };
  Vulkan.Cmd[Vulkan.CurrentFrame].bindVertexBuffers (
      0, 1, &Vulkan.VertexBuffer, offsets);*/

  Vulkan.Viewport.x      = 0;
  Vulkan.Viewport.y      = 0;
  Vulkan.Viewport.width  = static_cast<float> (Vulkan.SwapChainExtent.width);
  Vulkan.Viewport.height = static_cast<float> (Vulkan.SwapChainExtent.height);
  Vulkan.Viewport.minDepth = 0.0f;
  Vulkan.Viewport.maxDepth = 1.0f;
  Vulkan.Cmd[Vulkan.CurrentFrame].setViewport (0, 1, &Vulkan.Viewport);

  Vulkan.Scissor.offset = vk::Offset2D (0, 0);
  Vulkan.Scissor.extent = Vulkan.SwapChainExtent;
  Vulkan.Cmd[Vulkan.CurrentFrame].setScissor (0, 1, &Vulkan.Scissor);

  Vulkan.Cmd[Vulkan.CurrentFrame].draw (3, 1, 0, 0);

// Imgui New Frame
#if PATACHE_DEBUG == 1
  ImGui_ImplVulkan_NewFrame ();
  ImGui_ImplSDL3_NewFrame ();
  ImGui::NewFrame ();
  ImGui::SetNextWindowBgAlpha (0.0f);
  ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport ());
#endif

  return true;
}

#if PATACHE_DEBUG == 1
#include "DrawDebugUI.hpp"
#endif

void
Patache::Engine::EndRender (SDL_Event & Event)
{
#if PATACHE_DEBUG == 1
  // Imgui New Frame
  // ImGui::ShowDemoWindow ();
  Patache::DrawDebugUI (&engineInfo, configuration, Vulkan.SwapChainImageCount,
                        Vulkan.SwapChainExtent,
                        Vulkan.GraphicsQueueFamilyIndex);

  ImGui::Render ();

  if (engineInfo.ShowMainMenuBar || engineInfo.PatacheInfoWindow
      || engineInfo.PatacheConfigWindow)
    ImGui_ImplVulkan_RenderDrawData (
        ImGui::GetDrawData (),
        static_cast<VkCommandBuffer> (Vulkan.Cmd[Vulkan.CurrentFrame]),
        static_cast<VkPipeline> (Vulkan.ImguiPipeLine));
#endif

  // End RenderPass
  Vulkan.Cmd[Vulkan.CurrentFrame].endRenderPass ();

  // End Command Buffer
  vk::Result Result = Vulkan.Cmd[Vulkan.CurrentFrame].end ();

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "End Command Buffer #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  // Submit Queue
  {
    static constexpr vk::PipelineStageFlags WaitStages
        = vk::PipelineStageFlagBits::eTopOfPipe
          | vk::PipelineStageFlagBits::eColorAttachmentOutput;

    const vk::SubmitInfo SubmitInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &Vulkan.ImageAvailableSemaphore[Vulkan.CurrentFrame],
      .pWaitDstStageMask    = &WaitStages,
      .commandBufferCount   = 1,
      .pCommandBuffers      = &Vulkan.Cmd[Vulkan.CurrentFrame],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &Vulkan.ImageFinishedSemaphore[Vulkan.CurrentFrame]
    };

    Result = Vulkan.Queue.submit (1, &SubmitInfo,
                                  Vulkan.InFlightFences[Vulkan.CurrentFrame]);
  }

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Queue Submit 2", Result);
#endif

  // Present to surface
  const vk::PresentInfoKHR PresentInfo{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores    = &Vulkan.ImageFinishedSemaphore[Vulkan.CurrentFrame],
    .swapchainCount     = 1,
    .pSwapchains        = &Vulkan.SwapChain,
    .pImageIndices      = &Vulkan.ImageIndex,
  };

  Result = Vulkan.Queue.presentKHR (&PresentInfo);

#if PATACHE_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patache::Log::VulkanCheck, "Present KHR", Result);
#endif

  // Resize
  if (Result == vk::Result::eErrorOutOfDateKHR
      || Result == vk::Result::eSuboptimalKHR)
    {
      // Minimization
      vk::SurfaceCapabilitiesKHR Sc;
      vk::Result Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
          Vulkan.Surface, &Sc);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Get Surface Capabilities KHR", Result);

          return;
        }

      while (Sc.currentExtent.width <= 0 && Sc.currentExtent.height <= 0)
        {
          Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
              Vulkan.Surface, &Sc);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Get Surface Capabilities KHR", Result);

              return;
            }

          SDL_WaitEvent (&Event);
        }

      RecreateSwapChain (this);
    }

  Vulkan.CurrentFrame = (Vulkan.CurrentFrame + 1) % Vulkan.SwapChainImageCount;

#if defined(__linux__)
  wl_display_roundtrip (WaylandWindow.Display);
#endif
}
