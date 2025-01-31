#include "Vulkan_Render.hpp"

bool
Patata::Engine::BeginRender (SDL_Event & Event)
{
  // Wait Fences
  vk::Result Result
      = Vulkan.Device.waitForFences (1, &Vulkan.Fence, true, UINT64_MAX);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Wait For Fence", Result);
    }
#endif

  // Reset Fences
  Result = Vulkan.Device.resetFences (1, &Vulkan.Fence);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Reset Fences", Result);
    }
#endif
  
  // Acquire Next Image
  vk::AcquireNextImageInfoKHR NextImageInfo (
      Vulkan.SwapChain,        // swapchain
      UINT64_MAX,              // timeout
      Vulkan.AcquireSemaphore, // semaphore
      Vulkan.Fence,            // fence
      1,                       // deviceMask
      nullptr                  // pNext
  );

  Result = Vulkan.Device.acquireNextImage2KHR (&NextImageInfo,
                                               &Vulkan.ImageIndex);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Acquire Next Image 2 KHR", Result);
    }
#endif

  // Resize
  if (WindowResized || Result == vk::Result::eErrorOutOfDateKHR
      || Result == vk::Result::eSuboptimalKHR)
    {
      RecreateSwapChain (Event);
      WindowResized = false;
      return false;
    }

  vk::CommandBufferBeginInfo cmdBufferBeginInfo (
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit, // flags
      nullptr,                                        // pInheritanceInfo
      nullptr                                         // pNext
  );

  // Begin Command Buffer
  Result = Vulkan.cmd.begin (&cmdBufferBeginInfo);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Command Buffer Begin", Result);
    }
#endif

  // Begin RenderPass
  {
    vk::ClearColorValue Color{};
    Color.float32[0] = clearColor.r;
    Color.float32[1] = clearColor.g;
    Color.float32[2] = clearColor.b;
    Color.float32[3] = clearColor.a;

    vk::ClearValue ClearValue{};
    ClearValue.color = Color;

    vk::Rect2D renderArea{};
    renderArea.offset = vk::Offset2D{ 0, 0 };
    renderArea.extent = vk::Extent2D{ Vulkan.SwapChainExtent.width,
                                      Vulkan.SwapChainExtent.height };

    vk::RenderPassBeginInfo Info (
        Vulkan.RenderPass,                              // renderPass
        Vulkan.SwapChainFrameBuffer[Vulkan.ImageIndex], // framebuffer
        renderArea,                                     // renderArea
        1,                                              // clearValueCount
        &ClearValue,                                    // pClearValues
        nullptr                                         // pNext
    );

    Vulkan.cmd.beginRenderPass (Info, vk::SubpassContents::eInline);
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
        ImGui::GetDrawData (), static_cast<VkCommandBuffer> (Vulkan.cmd),
        static_cast<VkPipeline> (Vulkan.ImguiPipeLine));
#endif

  // End RenderPass
  Vulkan.cmd.endRenderPass ();

  // End Command Buffer
  vk::Result Result = Vulkan.cmd.end ();

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "End Command Buffer", Result);
    }
#endif
  
  Result = Vulkan.Device.waitForFences (1, &Vulkan.Fence, true, UINT64_MAX);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Wait For Adquire Image Fence", Result);
    }
#endif

  Result = Vulkan.Device.resetFences (1, &Vulkan.Fence);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Reset Fences", Result);
    }
#endif
  
  // Submit Queue
  {
    vk::PipelineStageFlags PipeLineStageFlags
        = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo SubmitInfo (1,                        // waitSemaphoreCount
                               &Vulkan.AcquireSemaphore, // pWaitSemaphores
                               &PipeLineStageFlags,      // pWaitDstStageMask
                               1,                        // commandBufferCount
                               &Vulkan.cmd,              // pCommandBuffers
                               1,                       // signalSemaphoreCount
                               &Vulkan.SubmitSemaphore, // pSignalSemaphores
                               nullptr                  // pNext
    );

    Result = Vulkan.Queue.submit (1, &SubmitInfo, Vulkan.Fence);
  }

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Queue Submit 2", Result);
    }
#endif

  // Present to surface
  vk::PresentInfoKHR PresentInfo (1, // waitSemaphoreCount
                                  &Vulkan.SubmitSemaphore, // waitSemaphores
                                  1,                       // swapchainCount
                                  &Vulkan.SwapChain,       // swapchains
                                  &Vulkan.ImageIndex,      // imageIndices
                                  nullptr,                 // results
                                  nullptr                  // pNext
  );

  Result = Vulkan.Queue.presentKHR (&PresentInfo);

#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Present KHR", Result);
    }
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
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patata::Log::VulkanCheck,
                            "Get Surface Capabilities KHR", Result);
        }

      if (Sc.currentExtent.width != Vulkan.SwapChainExtent.width
          || Sc.currentExtent.height != Vulkan.SwapChainExtent.height)
        RecreateSwapChain (Event);
    }
}
