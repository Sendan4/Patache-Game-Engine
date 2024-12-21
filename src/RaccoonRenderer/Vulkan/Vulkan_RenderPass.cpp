#include "Vulkan_RenderPass.hpp"

bool
Patata::RaccoonRenderer::CreateRenderPass (
    const Patata::SwapChainInfo & SwapChainInfo)
{
  vk::AttachmentDescription ColorAttachmentDescriptionInfo (
      {},
      SwapChainInfo.ImageColorFormat,   // format
      vk::SampleCountFlagBits::e1,      // samples
      vk::AttachmentLoadOp::eClear,     // loadOp
      vk::AttachmentStoreOp::eStore,    // storeOp
      vk::AttachmentLoadOp::eDontCare,  // stencilLoadOp
      vk::AttachmentStoreOp::eDontCare, // stencilStoreOp
      vk::ImageLayout::eUndefined,      // initialLayout
      vk::ImageLayout::ePresentSrcKHR   // finalLayout
  );

  vk::AttachmentReference ColorAttachmentRef (
      0,                                       // attachment
      vk::ImageLayout::eColorAttachmentOptimal // layout
  );

  vk::SubpassDescription SubpassDescriptionInfo (
      {},
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

  vk::RenderPassCreateInfo RenderPassCreateInfo (
      {},
      1,                               // attachmentCount
      &ColorAttachmentDescriptionInfo, // pAttachments
      1,                               // subpassCount
      &SubpassDescriptionInfo,         // pSubpasses
      0,                               // dependencyCount
      nullptr,                         // pDependencies
      nullptr                          // pNext
  );

  vk::Result Result = Vulkan.Device.createRenderPass (
      &RenderPassCreateInfo, nullptr, &Vulkan.RenderPass);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Render Pass", Result);

      return false;
    }
  else
    return true;
}
