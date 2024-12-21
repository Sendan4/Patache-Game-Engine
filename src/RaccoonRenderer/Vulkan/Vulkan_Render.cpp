#include "Vulkan_Render.hpp"

bool
Patata::RaccoonRenderer::BeginRender (SDL_Event & Event)
{
  vk::Result Result
      = Vulkan.Device.waitForFences (1, &Vulkan.Fence, true, UINT64_MAX);

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Wait For Fence", Result);
    }
#endif

  Result = Vulkan.Device.resetFences (1, &Vulkan.Fence);

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Reset Fences", Result);
    }
#endif

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

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Acquire Next Image 2 KHR", Result);
    }
#endif

  // Resize
  if (*pRaccoonInfo->pWindowResized || Result == vk::Result::eErrorOutOfDateKHR
      || Result == vk::Result::eSuboptimalKHR)
    {
      RecreateSwapChain (Event);
      *pRaccoonInfo->pWindowResized = false;
      return false;
    }

  vk::CommandBufferBeginInfo cmdBufferBeginInfo (
      vk::CommandBufferUsageFlagBits::eSimultaneousUse, // flags
      nullptr,                                          // pInheritanceInfo
      nullptr                                           // pNext
  );

  Result = Vulkan.cmd.begin (&cmdBufferBeginInfo);

#if defined(DEBUG)
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
    Color.float32[0] = pRaccoonInfo->pClearColor->r;
    Color.float32[1] = pRaccoonInfo->pClearColor->g;
    Color.float32[2] = pRaccoonInfo->pClearColor->b;
    Color.float32[3] = pRaccoonInfo->pClearColor->a;

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
#if defined(DEBUG)
  ImGui_ImplVulkan_NewFrame ();
  ImGui_ImplSDL2_NewFrame ();
  ImGui::NewFrame ();
  ImGui::SetNextWindowBgAlpha (0.0f);
  ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport ());
#endif

  return true;
}

#if defined(DEBUG)
#include "DrawDebugUI.hpp"
#endif

void
Patata::RaccoonRenderer::EndRender (SDL_Event & Event)
{
#if defined(DEBUG)
  // Imgui New Frame
  ImGui::ShowDemoWindow ();
  Patata::DrawDebugUI (pRaccoonInfo->pPatataEngineInfo,
                       pRaccoonInfo->pConfiguration,
                       Vulkan.SwapChainImageCount, Vulkan.SwapChainExtent);

  ImGui::Render ();
  ImGui_ImplVulkan_RenderDrawData (
      ImGui::GetDrawData (), static_cast<VkCommandBuffer> (Vulkan.cmd),
      static_cast<VkPipeline> (Vulkan.ImguiPipeLine));
#endif

  // End RenderPass
  Vulkan.cmd.endRenderPass ();

  // End Command Buffer
  vk::Result Result = Vulkan.cmd.end ();

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "End Command Buffer", Result);
    }
#endif

  Result = Vulkan.Device.waitForFences (1, &Vulkan.Fence, true, UINT64_MAX);

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Wait For Adquire Image Fence", Result);
    }
#endif

  Result = Vulkan.Device.resetFences (1, &Vulkan.Fence);

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Reset Fences", Result);
    }
#endif

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

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Queue Submit 2", Result);
    }
#endif

  vk::PresentInfoKHR PresentInfo (1, // waitSemaphoreCount
                                  &Vulkan.SubmitSemaphore, // waitSemaphores
                                  1,                       // swapchainCount
                                  &Vulkan.SwapChain,       // swapchains
                                  &Vulkan.ImageIndex,      // imageIndices
                                  nullptr,                 // results
                                  nullptr                  // pNext
  );

  Result = Vulkan.Queue.presentKHR (&PresentInfo);

#if defined(DEBUG)
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Present KHR", Result);
    }
#endif

  // Resize
  if (*pRaccoonInfo->pWindowResized
      || Result == vk::Result::eErrorOutOfDateKHR)
    {
      RecreateSwapChain (Event);
      *pRaccoonInfo->pWindowResized = false;
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
