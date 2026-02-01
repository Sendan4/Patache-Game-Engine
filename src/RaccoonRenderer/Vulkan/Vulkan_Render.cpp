#include "Vulkan_Render.hpp"

bool
Patache::Engine::BeginRender (SDL_Event & rEvent)
{
  // Wait Fences
  vulkan.renderResult = vulkan.device.waitForFences (
      1U, &vulkan.pInFlightFences[vulkan.currentFrame], VK_FALSE, UINT64_MAX);

  /*
   * Resizing Window is diferent in Linux with wayland
   * Wayland dont send vk::Result::eErrorOutOfDateKHR
   * The compositor throw the new size of window
   * and advise when you should resize.
   * */
#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
  if (resizingPending && resize)
    {
      RecreateSwapchain (this);

      resizingPending = false;
      resize          = false;

      vulkan.currentFrame = 0U;
      vulkan.imageIndex   = 0U;

      wl_surface_commit (waylandWindow.pSurface);
    }
#endif

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "vkWaitForFences() Fence #%.3u",
                     vulkan.currentFrame);

      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       errorText, vulkan.renderResult) };
    }
#endif

  // Acquire Next Image
  const vk::AcquireNextImageInfoKHR nextImageInfo{
    .swapchain  = vulkan.swapchain,
    .timeout    = UINT64_MAX,
    .semaphore  = vulkan.pImageAvailableSemaphores[vulkan.currentFrame],
    .fence      = VK_NULL_HANDLE,
    .deviceMask = 1U
  };

  vulkan.renderResult = vulkan.device.acquireNextImage2KHR (&nextImageInfo, &vulkan.imageIndex);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkAcquireNextImageKHR()",
                                                       vulkan.renderResult) };
    }
#endif

  // Resize
  if (vulkan.renderResult == vk::Result::eErrorOutOfDateKHR)
    {
      // Minimization
      vk::SurfaceCapabilitiesKHR sc;
      vk::Result result4 = vulkan.physicalDevice.getSurfaceCapabilitiesKHR (vulkan.surface, &sc);

      if (result4 != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanCheck{ std::async (
              std::launch::async, Patache::VulkanCheck,
              "vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result4) };

          return false;
        }

      while (sc.currentExtent.width <= 0U && sc.currentExtent.height <= 0U)
        {
          result4 = vulkan.physicalDevice.getSurfaceCapabilitiesKHR (vulkan.surface, &sc);

          if (result4 != vk::Result::eSuccess)
            {
              std::future<void> returnVulkanCheck{ std::async (
                  std::launch::async, Patache::VulkanCheck,
                  "vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result4) };

              return false;
            }

          SDL_WaitEvent (&rEvent);
        }

      RecreateSwapchain (this);
    }

  // Reset Fences
  vulkan.renderResult = vulkan.device.resetFences (1, &vulkan.pInFlightFences[vulkan.currentFrame]);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "vkResetFences() Fence #%.3u",
                     vulkan.currentFrame);

      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       errorText, vulkan.renderResult) };
    }
#endif

  // Reset Command Pool
  vulkan.renderResult = vulkan.device.resetCommandPool (
      vulkan.pCommandPools[vulkan.currentFrame], vk::CommandPoolResetFlagBits::eReleaseResources);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "vkResetCommandPool() Command Pool #%.3u", vulkan.currentFrame);

      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, errorText, vulkan.renderResult);
    }
#endif

  // Begin Command Buffer
  static constexpr vk::CommandBufferBeginInfo cmdBufferBeginInfo{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  vulkan.renderResult = vulkan.pCmd[vulkan.currentFrame].begin (&cmdBufferBeginInfo);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck = std::async (
          std::launch::async, Patache::VulkanCheck, "vkBeginCommandBuffer()", vulkan.renderResult);
    }
#endif

  if (vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset] > 0U
      && vulkan.physicalDeviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
      if (vulkan.copiesCount >= vulkan.swapchainImageCount)
        {
          // resetear la posicion del apuntador de memoria al inicio
          // para invalidad la memoria existente y sobreescribirla
          vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset] = 0U;
          vulkan.copiesCount                                              = 0U; // reset count

          goto EXIT_COPY;
        }

      vulkan.bufferCopy.srcOffset = 0U;
      vulkan.bufferCopy.dstOffset = 0U;
      vulkan.bufferCopy.size      = vulkan.stagingBufferInfo[Patache::VkBufferInfo::eSize];

      vulkan.pCmd[vulkan.currentFrame].copyBuffer (
          vulkan.stagingBuffer, vulkan.pRenderBuffer[vulkan.currentFrame], 1, &vulkan.bufferCopy);

      vulkan.bufferBarrier.srcAccessMask       = vk::AccessFlagBits::eTransferWrite;
      vulkan.bufferBarrier.dstAccessMask       = vk::AccessFlagBits::eVertexAttributeRead;
      vulkan.bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      vulkan.bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      vulkan.bufferBarrier.buffer              = vulkan.pRenderBuffer[vulkan.currentFrame];
      vulkan.bufferBarrier.offset              = 0U;
      vulkan.bufferBarrier.size = vulkan.renderBufferInfo[Patache::VkBufferInfo::eSize];

      vulkan.pCmd[vulkan.currentFrame].pipelineBarrier (
          vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eVertexInput, {}, 0,
          nullptr, 1, &vulkan.bufferBarrier, 0, nullptr);

      ++vulkan.copiesCount;
    }
EXIT_COPY:

  // Begin RenderPass
  {
    vulkan.clearColor.color.float32[0] = clearColor.r;
    vulkan.clearColor.color.float32[1] = clearColor.g;
    vulkan.clearColor.color.float32[2] = clearColor.b;
    vulkan.clearColor.color.float32[3] = clearColor.a;

    vulkan.renderArea.extent   = vulkan.swapchainExtent;
    vulkan.renderArea.offset.x = 0U;
    vulkan.renderArea.offset.y = 0U;

    const vk::RenderPassBeginInfo info{
      .renderPass      = vulkan.renderPass,
      .framebuffer     = vulkan.pSwapchainFrameBuffers[vulkan.imageIndex],
      .renderArea      = vulkan.renderArea,
      .clearValueCount = 1U,
      .pClearValues    = &vulkan.clearColor,
    };

    vulkan.pCmd[vulkan.currentFrame].beginRenderPass (info, vk::SubpassContents::eInline);
  }

  vulkan.pCmd[vulkan.currentFrame].bindPipeline (vk::PipelineBindPoint::eGraphics,
                                                 vulkan.graphicsPipeline);

  vulkan.viewport.x        = 0U;
  vulkan.viewport.y        = 0U;
  vulkan.viewport.width    = static_cast<float> (vulkan.swapchainExtent.width);
  vulkan.viewport.height   = static_cast<float> (vulkan.swapchainExtent.height);
  vulkan.viewport.minDepth = 0.0F;
  vulkan.viewport.maxDepth = 1.0F;
  vulkan.pCmd[vulkan.currentFrame].setViewport (0U, 1U, &vulkan.viewport);

  vulkan.scissor.offset = vk::Offset2D (0, 0);
  vulkan.scissor.extent = vulkan.swapchainExtent;
  vulkan.pCmd[vulkan.currentFrame].setScissor (0U, 1U, &vulkan.scissor);

// Imgui New Frame
#if PATACHE_DEBUG == 1
  ImGui_ImplVulkan_NewFrame ();
  ImGui_ImplSDL3_NewFrame ();
  ImGui::NewFrame ();
  ImGui::SetNextWindowBgAlpha (0.0F);
  ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport ());
#endif

  return true;
}

#if PATACHE_DEBUG == 1
  #include "DebugUI.hpp"
#endif

void
Patache::Engine::EndRender (SDL_Event & rEvent)
{
#if PATACHE_DEBUG == 1
  // Imgui New Frame
  // ImGui::ShowDemoWindow ();
  Patache::DrawDebugUI (this);
  ImGui::Render ();

  if ((debugInfo.showMainMenuBar || debugInfo.infoWindow || debugInfo.configWindow
       || debugInfo.raccoonRendererInfoWindow)
      && (ImGui::GetDrawData ()->TotalVtxCount > 0))
    {
      ImGui_ImplVulkan_RenderDrawData (
          ImGui::GetDrawData (), static_cast<VkCommandBuffer> (vulkan.pCmd[vulkan.currentFrame]),
          static_cast<VkPipeline> (vulkan.imguiPipeLine));
    }
#endif

  // End RenderPass
  vulkan.pCmd[vulkan.currentFrame].endRenderPass ();

  // End Command Buffer
  vulkan.renderResult = vulkan.pCmd[vulkan.currentFrame].end ();

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "vkEndCommandBuffer() Command Buffer #%.3u", vulkan.currentFrame);

      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       errorText, vulkan.renderResult) };
    }
#endif

  // Submit Queue
  {
    static constexpr vk::PipelineStageFlags waitStages
        = vk::PipelineStageFlagBits::eTopOfPipe | vk::PipelineStageFlagBits::eColorAttachmentOutput;

    const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1U,
                                     .pWaitSemaphores
                                     = &vulkan.pImageAvailableSemaphores[vulkan.currentFrame],
                                     .pWaitDstStageMask    = &waitStages,
                                     .commandBufferCount   = 1U,
                                     .pCommandBuffers      = &vulkan.pCmd[vulkan.currentFrame],
                                     .signalSemaphoreCount = 1U,
                                     .pSignalSemaphores
                                     = &vulkan.pImageFinishedSemaphores[vulkan.currentFrame] };

    vulkan.renderResult
        = vulkan.queue.submit (1, &submitInfo, vulkan.pInFlightFences[vulkan.currentFrame]);
  }

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (std::launch::async, Patache::VulkanCheck,
                                                       "vkQueueSubmit()", vulkan.renderResult) };
    }
#endif

  // Present to surface
  const vk::PresentInfoKHR presentInfo{
    .waitSemaphoreCount = 1U,
    .pWaitSemaphores    = &vulkan.pImageFinishedSemaphores[vulkan.currentFrame],
    .swapchainCount     = 1U,
    .pSwapchains        = &vulkan.swapchain,
    .pImageIndices      = &vulkan.imageIndex,
  };

  vulkan.renderResult = vulkan.queue.presentKHR (&presentInfo);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck{ std::async (
          std::launch::async, Patache::VulkanCheck, "vkQueuePresentKHR()", vulkan.renderResult) };
    }
#endif

  // Resize
  if (vulkan.renderResult == vk::Result::eErrorOutOfDateKHR
      || vulkan.renderResult == vk::Result::eSuboptimalKHR)
    {
      // Minimization
      vk::SurfaceCapabilitiesKHR sc;
      vk::Result result3 = vulkan.physicalDevice.getSurfaceCapabilitiesKHR (vulkan.surface, &sc);

      if (result3 != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanCheck{ std::async (
              std::launch::async, Patache::VulkanCheck,
              "vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result3) };

          return;
        }

      while (sc.currentExtent.width <= 0U && sc.currentExtent.height <= 0U)
        {
          result3 = vulkan.physicalDevice.getSurfaceCapabilitiesKHR (vulkan.surface, &sc);

          if (result3 != vk::Result::eSuccess)
            {
              std::future<void> returnVulkanCheck{ std::async (
                  std::launch::async, Patache::VulkanCheck,
                  "vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result3) };

              return;
            }

          SDL_WaitEvent (&rEvent);
        }

      RecreateSwapchain (this);
    }

  vulkan.currentFrame = (vulkan.currentFrame + 1U) % vulkan.swapchainImageCount;
}
