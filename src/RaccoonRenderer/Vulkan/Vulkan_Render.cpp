#include "Vulkan_Render.hpp"

bool
Patata::Engine::BeginRender (SDL_Event & Event)
{
  // Wait Fences
  vk::Result Result = Vulkan.Device.waitForFences (
      1, &Vulkan.InFlightFences[Vulkan.CurrentFrame], VK_TRUE, UINT64_MAX);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                     "Wait For Fence #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  // Acquire Next Image
  vk::AcquireNextImageInfoKHR NextImageInfo{
    .swapchain  = Vulkan.SwapChain,
    .timeout    = UINT64_MAX,
    .semaphore  = Vulkan.ImageAvailableSemaphore[Vulkan.CurrentFrame],
    .fence      = VK_NULL_HANDLE,
    .deviceMask = 1
  };

  Result = Vulkan.Device.acquireNextImage2KHR (&NextImageInfo,
                                               &Vulkan.ImageIndex);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Acquire Next Image 2 KHR", Result);
#endif

  // Resize
  if (WindowResized || Result == vk::Result::eErrorOutOfDateKHR
      || Result == vk::Result::eSuboptimalKHR)
    {
      RecreateSwapChain (Event);
      WindowResized = false;
      return false;
    }

  // Reset Fences
  Result = Vulkan.Device.resetFences (
      1, &Vulkan.InFlightFences[Vulkan.CurrentFrame]);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                     "Reset Fence #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  // Reset Command Buffer
  Vulkan.Cmd[Vulkan.CurrentFrame].reset (
      vk::CommandBufferResetFlagBits::eReleaseResources);

  static constexpr vk::CommandBufferBeginInfo cmdBufferBeginInfo{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  // Begin Command Buffer
  Result = Vulkan.Cmd[Vulkan.CurrentFrame].begin (&cmdBufferBeginInfo);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Command Buffer Begin", Result);
#endif

  // Begin RenderPass
  {
    // Render Pass
    Vulkan.Color.float32[0] = clearColor.r;
    Vulkan.Color.float32[1] = clearColor.g;
    Vulkan.Color.float32[2] = clearColor.b;
    Vulkan.Color.float32[3] = clearColor.a;

    Vulkan.RenderArea.extent = vk::Extent2D{ Vulkan.SwapChainExtent.width,
                                             Vulkan.SwapChainExtent.height };

    vk::RenderPassBeginInfo Info{
      .renderPass      = Vulkan.RenderPass,
      .framebuffer     = Vulkan.SwapChainFrameBuffer[Vulkan.ImageIndex],
      .renderArea      = Vulkan.RenderArea,
      .clearValueCount = 1,
      .pClearValues    = &Vulkan.ClearValue,
    };

    Vulkan.Cmd[Vulkan.CurrentFrame].beginRenderPass (
        Info, vk::SubpassContents::eInline);
  }

// Imgui New Frame
#if PATATA_DEBUG == 1
  ImGui_ImplVulkan_NewFrame ();
  ImGui_ImplSDL2_NewFrame ();
  ImGui::NewFrame ();
  ImGui::SetNextWindowBgAlpha (0.0f);
  ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport ());
#endif

  return true;
}

#if PATATA_DEBUG == 1
#include "DrawDebugUI.hpp"
#endif

void
Patata::Engine::EndRender (SDL_Event & Event)
{
#if PATATA_DEBUG == 1
  // Imgui New Frame
  // ImGui::ShowDemoWindow ();
  Patata::DrawDebugUI (&engineInfo, configuration, Vulkan.SwapChainImageCount,
                       Vulkan.SwapChainExtent);

  ImGui::Render ();

  if (engineInfo.ShowMainMenuBar || engineInfo.PatataInfoWindow
      || engineInfo.PatataConfigWindow)
    ImGui_ImplVulkan_RenderDrawData (
        ImGui::GetDrawData (),
        static_cast<VkCommandBuffer> (Vulkan.Cmd[Vulkan.CurrentFrame]),
        static_cast<VkPipeline> (Vulkan.ImguiPipeLine));
#endif

  // End RenderPass
  Vulkan.Cmd[Vulkan.CurrentFrame].endRenderPass ();

  // End Command Buffer
  vk::Result Result = Vulkan.Cmd[Vulkan.CurrentFrame].end ();

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                     "End Command Buffer #%.3u", Vulkan.CurrentFrame);

      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, ErrorText, Result);
    }
#endif

  // Submit Queue
  {
    static constexpr vk::PipelineStageFlags WaitStages
        = vk::PipelineStageFlagBits::eTopOfPipe
          | vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo SubmitInfo{
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

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patata::Log::VulkanCheck,
                      "Queue Submit 2", Result);
#endif

  // Present to surface
  vk::PresentInfoKHR PresentInfo{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores    = &Vulkan.ImageFinishedSemaphore[Vulkan.CurrentFrame],
    .swapchainCount     = 1,
    .pSwapchains        = &Vulkan.SwapChain,
    .pImageIndices      = &Vulkan.ImageIndex,
  };

  Result = Vulkan.Queue.presentKHR (&PresentInfo);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck = std::async (
        std::launch::async, Patata::Log::VulkanCheck, "Present KHR", Result);
#endif

  // Resize
  if (WindowResized || Result == vk::Result::eErrorOutOfDateKHR)
    {
      RecreateSwapChain (Event);
      WindowResized = false;
    }
  else if (Result == vk::Result::eSuboptimalKHR)
    {
      vk::SurfaceCapabilitiesKHR Sc;
      Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (Vulkan.Surface,
                                                                &Sc);

      if (Result != vk::Result::eSuccess)
        std::future<void> ReturnVulkanCheck
            = std::async (std::launch::async, Patata::Log::VulkanCheck,
                          "Get Surface Capabilities KHR", Result);

      if (Sc.currentExtent.width != Vulkan.SwapChainExtent.width
          || Sc.currentExtent.height != Vulkan.SwapChainExtent.height)
        RecreateSwapChain (Event);
    }

  Vulkan.CurrentFrame = (Vulkan.CurrentFrame + 1) % Vulkan.SwapChainImageCount;
}
