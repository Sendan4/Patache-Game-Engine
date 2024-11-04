#include "Vulkan_Render.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::BeginVulkanRender (SDL_Event & Event)
{
    vk::Result Result = Device.waitForFences (1, &Fence, true, std::numeric_limits<uint64_t>::max ());

    #if defined(DEBUG)
    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
            Patata::Log::VulkanCheck, "Wait For Fence", Result);
      }
    #endif

    Result = Device.resetFences (1, &Fence);

    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess)
	  {
	    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
			Patata::Log::VulkanCheck, "Reset Fences", Result);
      }
    #endif

	vk::AcquireNextImageInfoKHR NextImageInfo (
	    SwapChain,                             // swapchain
		std::numeric_limits<uint64_t>::max (), // timeout
		AcquireSemaphore,                      // semaphore
		Fence,                                 // fence
		1,                                     // deviceMask
		nullptr                                // pNext
	);

    Result = Device.acquireNextImage2KHR(&NextImageInfo, &ImageIndex);

    #if defined(DEBUG)
    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
            Patata::Log::VulkanCheck, "Acquire Next Image 2 KHR", Result);
      }
    #endif

    // Resize
    if (*pRaccoonInfo->pWindowResized || Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eSuboptimalKHR)
      {
        RecreateSwapChain(Event);
        *pRaccoonInfo->pWindowResized = false;
        return false;
      }

    vk::CommandBufferBeginInfo cmdBufferBeginInfo (
        vk::CommandBufferUsageFlagBits::eSimultaneousUse, // flags
        nullptr,                                          // pInheritanceInfo
        nullptr                                           // pNext
    );

    Result = cmd.begin(&cmdBufferBeginInfo);

    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess)
	  {
	    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
			Patata::Log::VulkanCheck, "Command Buffer Begin", Result);
      }
    #endif

    CmdIsReady = true;

    // Begin RenderPass
    {
        vk::ClearColorValue Color {};
        Color.float32[0] = pRaccoonInfo->pClearColor->r;
        Color.float32[1] = pRaccoonInfo->pClearColor->g;
        Color.float32[2] = pRaccoonInfo->pClearColor->b;
        Color.float32[3] = pRaccoonInfo->pClearColor->a;

        vk::ClearValue ClearValue {};
        ClearValue.color = Color;

        vk::Rect2D renderArea {};
        renderArea.offset = vk::Offset2D { 0, 0 };
        renderArea.extent = vk::Extent2D { SwapChainExtent.width, SwapChainExtent.height };

        vk::RenderPassBeginInfo Info (
            RenderPass,                       // renderPass
            SwapChainFrameBuffer[ImageIndex], // framebuffer
            renderArea,                       // renderArea
            1,                                // clearValueCount
            &ClearValue,                      // pClearValues
            nullptr                           // pNext
        );

        cmd.beginRenderPass(Info, vk::SubpassContents::eInline);
    }

    // Imgui New Frame
    #if defined (DEBUG)
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    #endif

    return true;
}

#if defined (DEBUG)
#include "DrawDebugUI.hpp"
#endif

void
Patata::Graphics::RaccoonRenderer::VulkanBackend::EndVulkanRender (SDL_Event & Event)
{
    if(!CmdIsReady) return;

    // Imgui New Frame
    #if defined (DEBUG)
    ImGui::ShowDemoWindow();
    Patata::DrawDebugUI(
        pRaccoonInfo->pPatataEngineInfo,
        SwapChainImageCount,
        SwapChainExtent);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(
		ImGui::GetDrawData(),
		static_cast<VkCommandBuffer>(cmd),
		static_cast<VkPipeline>(ImguiPipeLine)
	);
    #endif

    // End RenderPass
    cmd.endRenderPass();

    // End Command Buffer
    vk::Result Result = cmd.end ();

    #if defined(DEBUG)
    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
            Patata::Log::VulkanCheck, "End Command Buffer", Result);
      }
    #endif

    CmdIsReady = false;

    Result = Device.waitForFences (1, &Fence, true, std::numeric_limits<uint64_t>::max ());

    #if defined(DEBUG)
    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
            Patata::Log::VulkanCheck, "Wait For Adquire Image Fence", Result);
      }
    #endif

    Result = Device.resetFences (1, &Fence);

    #if defined(DEBUG)
    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
            Patata::Log::VulkanCheck, "Reset Fences", Result);
      }
    #endif

    vk::PipelineStageFlags PipeLineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo SubmitInfo (
        1,                   // waitSemaphoreCount
        &AcquireSemaphore,   // pWaitSemaphores
        &PipeLineStageFlags, // pWaitDstStageMask
        1,                   // commandBufferCount
        &cmd,                // pCommandBuffers
        1,                   // signalSemaphoreCount
        &SubmitSemaphore,    // pSignalSemaphores
        nullptr              // pNext
    );

    Result = Queue.submit (1, &SubmitInfo, Fence);

    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess)
	  {
	    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
			Patata::Log::VulkanCheck, "Queue Submit 2", Result);
      }
    #endif

    vk::PresentInfoKHR PresentInfo (
        1,                // waitSemaphoreCount
        &SubmitSemaphore, // waitSemaphores
        1,                // swapchainCount
        &SwapChain,       // swapchains
        &ImageIndex,      // imageIndices
        nullptr,          // results
        nullptr           // pNext
    );

    Result = Queue.presentKHR(&PresentInfo);

    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
			Patata::Log::VulkanCheck, "Present KHR", Result);
    }
    #endif

    // Resize
    if (*pRaccoonInfo->pWindowResized || Result == vk::Result::eErrorOutOfDateKHR)
      {
        RecreateSwapChain (Event);
        *pRaccoonInfo->pWindowResized = false;
      }
    else if (Result == vk::Result::eSuboptimalKHR)
      {
          vk::SurfaceCapabilitiesKHR Sc;
          Result = PhysicalDevice.getSurfaceCapabilitiesKHR(Surface, &Sc);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
                  Patata::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);
            }

          if (Sc.currentExtent.width != SwapChainExtent.width || Sc.currentExtent.height != SwapChainExtent.height)
              RecreateSwapChain(Event);
      }
}
