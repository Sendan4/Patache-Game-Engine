#include "Vulkan_RenderPass.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateRenderPass (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> & SwapChainInfo)
{
    vk::AttachmentDescription ColorAttachmentDescriptionInfo {};
    ColorAttachmentDescriptionInfo.format = std::get<1>(SwapChainInfo);
    ColorAttachmentDescriptionInfo.samples = vk::SampleCountFlagBits::e1;
    ColorAttachmentDescriptionInfo.loadOp = vk::AttachmentLoadOp::eClear;
    ColorAttachmentDescriptionInfo.storeOp = vk::AttachmentStoreOp::eStore;
    ColorAttachmentDescriptionInfo.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    ColorAttachmentDescriptionInfo.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    ColorAttachmentDescriptionInfo.initialLayout = vk::ImageLayout::eUndefined;
    ColorAttachmentDescriptionInfo.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference ColorAttachmentRef {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription SubpassDescriptionInfo {};
    SubpassDescriptionInfo.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    SubpassDescriptionInfo.colorAttachmentCount = 1;
    SubpassDescriptionInfo.pColorAttachments = &ColorAttachmentRef;

    vk::RenderPassCreateInfo RenderPassCreateInfo {};
    RenderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    RenderPassCreateInfo.attachmentCount = 1;
    RenderPassCreateInfo.pAttachments = &ColorAttachmentDescriptionInfo;
    RenderPassCreateInfo.subpassCount = 1;
    RenderPassCreateInfo.pSubpasses = &SubpassDescriptionInfo;

    vk::Result Result = Device.createRenderPass(&RenderPassCreateInfo, nullptr, &RenderPass);
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Render Pass", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}
