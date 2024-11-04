#include "Vulkan_RenderPass.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateRenderPass (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> & SwapChainInfo)
{
    vk::AttachmentDescription ColorAttachmentDescriptionInfo ({},
        std::get<1>(SwapChainInfo),       // format
        vk::SampleCountFlagBits::e1,      // samples
        vk::AttachmentLoadOp::eClear,     // loadOp
        vk::AttachmentStoreOp::eStore,    // storeOp
        vk::AttachmentLoadOp::eDontCare,  // stencilLoadOp
        vk::AttachmentStoreOp::eDontCare, // stencilStoreOp
        vk::ImageLayout::eUndefined,      // initialLayout
        vk::ImageLayout::ePresentSrcKHR   // finalLayout
    );

    vk::AttachmentReference ColorAttachmentRef (
        0,                                        // attachment
        vk::ImageLayout::eColorAttachmentOptimal  // layout
    );

    vk::SubpassDescription SubpassDescriptionInfo ({},
        vk::PipelineBindPoint::eGraphics, // pipelineBindPoint
        0,                                // inputAttachmentCount
        nullptr,                          // pInputAttachments
        1,                                // colorAttachmentCount
        &ColorAttachmentRef,              // pColorAttachments
        nullptr,                          // pResolveAttachments
        nullptr,                          // pDepthStencilAttachment
        0,                                // preserveAttachmentCount
        nullptr                           // pPreserveAttachments
    );

    vk::RenderPassCreateInfo RenderPassCreateInfo ({},
        1,                               // attachmentCount
        &ColorAttachmentDescriptionInfo, // pAttachments
        1,                               // subpassCount
        &SubpassDescriptionInfo,         // pSubpasses
        0,                               // dependencyCount
        nullptr,                         // pDependencies
        nullptr                          // pNext
    );

    vk::Result Result = Device.createRenderPass(&RenderPassCreateInfo, nullptr, &RenderPass);
    {
      std::future<void> ReturnVulkanCheck = std::async (std::launch::async,
          Patata::Log::VulkanCheck, "Render Pass", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}
