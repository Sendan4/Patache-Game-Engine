#include "Vulkan_FrameBuffer.hpp"

bool Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateFrameBuffer(void)
{
    fast_io::io::println (
        #if defined(_WIN64)
        fast_io::out (),
        #endif
        PATATA_TERM_BOLD,
        "\nSwapChain Frame Buffer : ",
        PATATA_TERM_RESET,
        SwapChainImageCount);

    SwapChainFrameBuffer = new vk::Framebuffer[SwapChainImageCount];

    vk::Result Result;

    for (uint8_t i = 0; i < SwapChainImageCount; ++i) {
        vk::FramebufferCreateInfo FrameBufferInfo ({},
            RenderPass,                  // renderPass
            1,                           // attachmentCount
            &SwapChainColorImageView[i], // pAttachments
            SwapChainExtent.width,       // width
            SwapChainExtent.height,      // height
            1,                           // layers
            nullptr                      // pNext
        );

        Result = Device.createFramebuffer(&FrameBufferInfo, nullptr, &SwapChainFrameBuffer[i]);
        {
            fast_io::io::print (
                #if defined(_WIN64)
                fast_io::out (),
                #endif
                "  ");

            std::future<void> ReturnVulkanCheck
                = std::async (std::launch::async, Patata::Log::VulkanCheck, "Frame Buffer", Result);
        }

        if (Result != vk::Result::eSuccess) return false;
    }

    fast_io::io::println (
        #if defined(_WIN64)
        fast_io::out (),
        #endif
        "");

    return true;
}
