#include "Vulkan_Render.hpp"

bool
Patache::Engine::BeginRender (SDL_Event & rEvent)
{
#if PATACHE_DEBUG == 1
  if (debugInfo.timerFpsRun && debugInfo.calculatePerformanceStats)
    {
      debugInfo.frameTimer  = std::chrono::high_resolution_clock::now ();
      debugInfo.timerFpsRun = false;
    }
#endif

  // Wait Fences
  vulkan.renderResult = vkWaitForFences (
      vulkan.device, 1U, &vulkan.pInFlightFences[vulkan.currentFrame], VK_FALSE, UINT64_MAX);

  /*
   * Resizing Window is diferent in Linux with wayland
   * Wayland dont send vk::Result::eErrorOutOfDateKHR
   * The compositor throw the new size of window
   * and advise when you should resize.
   * */
#if PATACHE_LINUX_OR_UNIX
  if (resizingPending && resize)
    {
  #if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTimer = std::chrono::high_resolution_clock::now ();
        }
  #endif
      Patache::RecreateSwapchain (this);
  #if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTime
              = std::chrono::duration<float, std::chrono::milliseconds::period> (
                    std::chrono::high_resolution_clock::now () - debugInfo.mainRenderPassTimer)
                    .count ();
        }
  #endif

      resizingPending = false;
      resize          = false;

      wl_surface_commit (waylandWindow.pSurface);
    }
#endif

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "vkWaitForFences() Fence #%.3u",
                     vulkan.currentFrame);

      Patache::VulkanCheck (errorText, vulkan.renderResult);
    }
#endif

  // Acquire Next Image
  const VkAcquireNextImageInfoKHR nextImageInfo{
    .sType      = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
    .pNext      = nullptr,
    .swapchain  = vulkan.swapchain,
    .timeout    = UINT64_MAX,
    .semaphore  = vulkan.pImageAvailableSemaphores[vulkan.currentFrame],
    .fence      = VK_NULL_HANDLE,
    .deviceMask = 1U
  };

  vulkan.renderResult = vkAcquireNextImage2KHR (vulkan.device, &nextImageInfo, &vulkan.imageIndex);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkAcquireNextImage2KHR()", vulkan.renderResult);
    }
#endif

  // Resize
  if (vulkan.renderResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // Minimization
      VkSurfaceCapabilitiesKHR sc{};
      VkResult result4{ vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vulkan.physicalDevice,
                                                                   vulkan.surface, &sc) };

      if (result4 != VK_SUCCESS)
        {
          Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result4);

          return false;
        }

      while (sc.currentExtent.width <= 0U && sc.currentExtent.height <= 0U)
        {
          result4 = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vulkan.physicalDevice,
                                                               vulkan.surface, &sc);

          if (result4 != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result4);

              return false;
            }

          SDL_WaitEvent (&rEvent);
        }

#if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTimer = std::chrono::high_resolution_clock::now ();
        }
#endif
      Patache::RecreateSwapchain (this);
#if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTime
              = std::chrono::duration<float, std::chrono::milliseconds::period> (
                    std::chrono::high_resolution_clock::now () - debugInfo.mainRenderPassTimer)
                    .count ();
        }
#endif
    }

  // Reset Fences
  vulkan.renderResult
      = vkResetFences (vulkan.device, 1U, &vulkan.pInFlightFences[vulkan.currentFrame]);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1, "vkResetFences() Fence #%.3u",
                     vulkan.currentFrame);

      Patache::VulkanCheck (errorText, vulkan.renderResult);
    }
#endif

  // Reset Command Pool
  vulkan.renderResult
      = vkResetCommandPool (vulkan.device, vulkan.pCommandPools[vulkan.currentFrame],
                            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "vkResetCommandPool() Command Pool #%.3u", vulkan.currentFrame);

      Patache::VulkanCheck (errorText, vulkan.renderResult);
    }
#endif

  // Begin Command Buffer
  static constexpr VkCommandBufferBeginInfo cmdBufferBeginInfo{
    .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext            = nullptr,
    .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = nullptr
  };

  vulkan.renderResult
      = vkBeginCommandBuffer (vulkan.pCmd[vulkan.currentFrame], &cmdBufferBeginInfo);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkBeginCommandBuffer()", vulkan.renderResult);
    }

  if (debugInfo.calculatePerformanceStats)
    {
      debugInfo.mainCmdTimer = std::chrono::high_resolution_clock::now ();
    }
#endif

  if (vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset] > 0U
      && vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
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

      vkCmdCopyBuffer (vulkan.pCmd[vulkan.currentFrame], vulkan.stagingBuffer,
                       vulkan.pRenderBuffer[vulkan.currentFrame], 1U, &vulkan.bufferCopy);

      vulkan.bufferBarrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
      vulkan.bufferBarrier.dstAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
      vulkan.bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      vulkan.bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      vulkan.bufferBarrier.buffer              = vulkan.pRenderBuffer[vulkan.currentFrame];
      vulkan.bufferBarrier.offset              = 0U;
      vulkan.bufferBarrier.size = vulkan.renderBufferInfo[Patache::VkBufferInfo::eSize];

      vkCmdPipelineBarrier (vulkan.pCmd[vulkan.currentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0U,
                            nullptr, 1U, &vulkan.bufferBarrier, 0U, nullptr);

      ++vulkan.copiesCount;
    }
EXIT_COPY:

  // Begin RenderPass
  {
    vulkan.clearColor.color.float32[0] = clearColor.r;
    vulkan.clearColor.color.float32[1] = clearColor.g;
    vulkan.clearColor.color.float32[2] = clearColor.b;
    vulkan.clearColor.color.float32[3] = clearColor.a;

#if PATACHE_LINUX_OR_UNIX
    vulkan.renderArea.extent.width  = vulkan.swapchainExtent.width * scaleInt;
    vulkan.renderArea.extent.height = vulkan.swapchainExtent.height * scaleInt;
#else
    vulkan.renderArea.extent.width  = vulkan.swapchainExtent.width;
    vulkan.renderArea.extent.height = vulkan.swapchainExtent.height;
#endif

    vulkan.renderArea.offset.x = 0U;
    vulkan.renderArea.offset.y = 0U;

    const VkRenderPassBeginInfo info{
      .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext           = nullptr,
      .renderPass      = vulkan.renderPass,
      .framebuffer     = vulkan.pSwapchainFrameBuffers[vulkan.imageIndex],
      .renderArea      = vulkan.renderArea,
      .clearValueCount = 1U,
      .pClearValues    = &vulkan.clearColor,
    };

    vkCmdBeginRenderPass (vulkan.pCmd[vulkan.currentFrame], &info, VK_SUBPASS_CONTENTS_INLINE);

#if PATACHE_DEBUG == 1
    if (debugInfo.calculatePerformanceStats)
      {
        debugInfo.mainRenderPassTimer = std::chrono::high_resolution_clock::now ();
      }
#endif
  }

  vkCmdBindPipeline (vulkan.pCmd[vulkan.currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                     vulkan.graphicsPipeline);

  vulkan.viewport.x = 0U;
  vulkan.viewport.y = 0U;
#if PATACHE_LINUX_OR_UNIX
  vulkan.viewport.width  = static_cast<float> (vulkan.swapchainExtent.width * scaleInt);
  vulkan.viewport.height = static_cast<float> (vulkan.swapchainExtent.height * scaleInt);
#else
  vulkan.viewport.width  = static_cast<float> (vulkan.swapchainExtent.width);
  vulkan.viewport.height = static_cast<float> (vulkan.swapchainExtent.height);
#endif
  vulkan.viewport.minDepth = 0.0F;
  vulkan.viewport.maxDepth = 1.0F;
  vkCmdSetViewport (vulkan.pCmd[vulkan.currentFrame], 0U, 1U, &vulkan.viewport);

  vulkan.scissor.offset.x = 0U;
  vulkan.scissor.offset.y = 0U;
#if PATACHE_LINUX_OR_UNIX
  vulkan.scissor.extent.width  = vulkan.swapchainExtent.width * scaleInt;
  vulkan.scissor.extent.height = vulkan.swapchainExtent.height * scaleInt;
#else
  vulkan.scissor.extent.width  = vulkan.swapchainExtent.width;
  vulkan.scissor.extent.height = vulkan.swapchainExtent.height;
#endif
  vkCmdSetScissor (vulkan.pCmd[vulkan.currentFrame], 0U, 1U, &vulkan.scissor);

// Imgui New Frame
#if PATACHE_DEBUG == 1
  ImGui_ImplVulkan_NewFrame ();
  ImGui_ImplSDL3_NewFrame ();
  ImGui::NewFrame ();
  ImGui::SetNextWindowBgAlpha (0.0F);

  #if PATACHE_LINUX_OR_UNIX
  ImGui::GetMainViewport ()->Size
      = ImVec2 (vulkan.swapchainExtent.width * scaleInt, vulkan.swapchainExtent.height * scaleInt);

  ImGui::GetMainViewport ()->FramebufferScale = ImVec2 (scaleInt, scaleInt);
  #endif

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
  //ImGui::ShowDemoWindow ();
  Patache::DrawDebugUI (this);
  ImGui::Render ();

  if ((debugInfo.showMainMenuBar || debugInfo.infoWindow || debugInfo.configWindow
       || debugInfo.raccoonRendererInfoWindow)
      && (ImGui::GetDrawData ()->TotalVtxCount > 0))
    {
      ImGui_ImplVulkan_RenderDrawData (ImGui::GetDrawData (), vulkan.pCmd[vulkan.currentFrame],
                                       vulkan.imguiPipeLine);
    }
#endif

  // End RenderPass
  vkCmdEndRenderPass (vulkan.pCmd[vulkan.currentFrame]);

#if PATACHE_DEBUG == 1
  if (debugInfo.calculatePerformanceStats)
    {
      debugInfo.mainRenderPassTime
          = std::chrono::duration<float, std::chrono::milliseconds::period> (
                std::chrono::high_resolution_clock::now () - debugInfo.mainRenderPassTimer)
                .count ();
    }
#endif

  // End Command Buffer
  vulkan.renderResult = vkEndCommandBuffer (vulkan.pCmd[vulkan.currentFrame]);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

      std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                     "vkEndCommandBuffer() Command Buffer #%.3u", vulkan.currentFrame);

      Patache::VulkanCheck (errorText, vulkan.renderResult);
    }

  if (debugInfo.calculatePerformanceStats)
    {
      debugInfo.mainCmdTime
          = std::chrono::duration<float, std::chrono::milliseconds::period> (
                std::chrono::high_resolution_clock::now () - debugInfo.mainCmdTimer)
                .count ();
    }
#endif

  // Submit Queue
  {
    static constexpr VkPipelineStageFlags waitStages{ static_cast<VkPipelineStageFlagBits> (
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT) };

    const VkSubmitInfo submitInfo{ .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                   .pNext              = nullptr,
                                   .waitSemaphoreCount = 1U,
                                   .pWaitSemaphores
                                   = &vulkan.pImageAvailableSemaphores[vulkan.currentFrame],
                                   .pWaitDstStageMask    = &waitStages,
                                   .commandBufferCount   = 1U,
                                   .pCommandBuffers      = &vulkan.pCmd[vulkan.currentFrame],
                                   .signalSemaphoreCount = 1U,
                                   .pSignalSemaphores
                                   = &vulkan.pImageFinishedSemaphores[vulkan.currentFrame] };

    vulkan.renderResult = vkQueueSubmit (vulkan.queue, 1U, &submitInfo,
                                         vulkan.pInFlightFences[vulkan.currentFrame]);
  }

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkQueueSubmit()", vulkan.renderResult);
    }
#endif

  // Present to surface
  const VkPresentInfoKHR presentInfo{ .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                      .pNext              = nullptr,
                                      .waitSemaphoreCount = 1U,
                                      .pWaitSemaphores
                                      = &vulkan.pImageFinishedSemaphores[vulkan.currentFrame],
                                      .swapchainCount = 1U,
                                      .pSwapchains    = &vulkan.swapchain,
                                      .pImageIndices  = &vulkan.imageIndex,
                                      .pResults       = nullptr };

  vulkan.renderResult = vkQueuePresentKHR (vulkan.queue, &presentInfo);

#if PATACHE_DEBUG == 1
  if (vulkan.renderResult != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkQueuePresentKHR()", vulkan.renderResult);
    }
#endif

  // Resize
  if (vulkan.renderResult == VK_ERROR_OUT_OF_DATE_KHR || vulkan.renderResult == VK_SUBOPTIMAL_KHR)
    {
      // Minimization
      VkSurfaceCapabilitiesKHR sc{};
      VkResult result3{ vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vulkan.physicalDevice,
                                                                   vulkan.surface, &sc) };

      if (result3 != VK_SUCCESS)
        {
          Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result3);

          return;
        }

      while (sc.currentExtent.width <= 0U && sc.currentExtent.height <= 0U)
        {
          result3 = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vulkan.physicalDevice,
                                                               vulkan.surface, &sc);

          if (result3 != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result3);

              return;
            }

          SDL_WaitEvent (&rEvent);
        }

#if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTimer = std::chrono::high_resolution_clock::now ();
        }
#endif
      Patache::RecreateSwapchain (this);
#if PATACHE_DEBUG == 1
      if (debugInfo.calculatePerformanceStats)
        {
          debugInfo.mainSwapchainTime
              = std::chrono::duration<float, std::chrono::milliseconds::period> (
                    std::chrono::high_resolution_clock::now () - debugInfo.mainRenderPassTimer)
                    .count ();
        }
#endif
    }

  vulkan.currentFrame = (vulkan.currentFrame + 1U) % vulkan.swapchainImageCount;

#if PATACHE_DEBUG == 1
  ++debugInfo.numFrames;
#endif
}
