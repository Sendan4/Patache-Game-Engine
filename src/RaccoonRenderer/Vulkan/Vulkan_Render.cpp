#include "Vulkan_Render.hpp"

void
Patata::Graphics::RaccoonRenderer::VulkanBackend::BeginVulkanRender (void)
{
    vk::Result Result = Device.waitForFences(1, &Fence, true, std::numeric_limits<uint64_t>::max());
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Wait For Fences", Result);
    }
    #endif

    Result = Device.resetFences(1, &Fence);
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Reset Fences", Result);
    }
    #endif

	vk::AcquireNextImageInfoKHR NextImageInfo {};
	NextImageInfo.sType = vk::StructureType::eAcquireNextImageInfoKHR;
	NextImageInfo.pNext = nullptr;
	NextImageInfo.swapchain = SwapChain;
	NextImageInfo.semaphore = AcquireSemaphore;
	NextImageInfo.deviceMask = 1;
    NextImageInfo.timeout    = std::numeric_limits<uint64_t>::max ();
	NextImageInfo.fence = Fence;

	Result = Device.acquireNextImage2KHR(&NextImageInfo, &ImageIndex);
	#if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Acquire Next Image 2 KHR", Result);
    }
    #endif

    if (*pWindowResized || Result == vk::Result::eErrorOutOfDateKHR
            || Result == vk::Result::eSuboptimalKHR)
    {
        fast_io::io::println (
            #if defined(_WIN64)
            fast_io::out(),
            #endif
            "\n", PATATA_TERM_BOLD,
            PATATA_TERM_COLOR_PATATA,
            "Raccoon Renderer", PATATA_TERM_RESET,
            " : Recreating SwapChain");

        RecreateSwapChain();

        #if defined(__linux__)
        *pWindowResized = false;
        #endif

        return;
    }

    vk::CommandBufferAllocateInfo cmdAllocateInfo {};
    cmdAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    cmdAllocateInfo.pNext = nullptr;
    cmdAllocateInfo.commandPool = CommandPool;
    cmdAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdAllocateInfo.commandBufferCount = 1;

    Result = Device.allocateCommandBuffers(&cmdAllocateInfo, &cmd);
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Allocate Command Buffers", Result);
    }
    #endif

    vk::CommandBufferBeginInfo cmdBufferBeginInfo {};
    cmdBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    cmdBufferBeginInfo.pNext = nullptr;
    cmdBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    Result = cmd.begin(&cmdBufferBeginInfo);
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Command Buffer Begin", Result);
    }
    #endif

    CmdIsReady = true;
}

void
Patata::Graphics::RaccoonRenderer::VulkanBackend::EndVulkanRender (void)
{
    if(!CmdIsReady) return;

    cmd.end();

    CmdIsReady = false;

    vk::PipelineStageFlags PipeLineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo SubmitInfo {};
    SubmitInfo.sType = vk::StructureType::eSubmitInfo;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &cmd;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &SubmitSemaphore;
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &AcquireSemaphore;
    SubmitInfo.pWaitDstStageMask = &PipeLineStageFlags;

    vk::Result Result = Queue.submit(1, &SubmitInfo, nullptr);
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Queue Submit 2", Result);
    }
    #endif

    vk::PresentInfoKHR PresentInfo {};
    PresentInfo.sType = vk::StructureType::ePresentInfoKHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &SwapChain;
    PresentInfo.pImageIndices = &ImageIndex;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &SubmitSemaphore;

    Result = Queue.presentKHR(&PresentInfo);
    #if defined(DEBUG)
	if (Result != vk::Result::eSuccess) {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Present KHR", Result);
    }
    #endif

    if (*pWindowResized || Result == vk::Result::eErrorOutOfDateKHR
        || Result == vk::Result::eSuboptimalKHR)
      {
        fast_io::io::println (
            #if defined(_WIN64)
            fast_io::out(),
            #endif
            "\n", PATATA_TERM_BOLD,
            PATATA_TERM_COLOR_PATATA,
            "Raccoon Renderer", PATATA_TERM_RESET,
            " : Recreating SwapChain");

        RecreateSwapChain();

        *pWindowResized = false;
    }

    Device.waitIdle();

    Queue.waitIdle();

    Device.freeCommandBuffers (CommandPool, 1, &cmd);
}
