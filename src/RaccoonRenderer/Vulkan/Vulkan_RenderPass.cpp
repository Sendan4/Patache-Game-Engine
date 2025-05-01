#include "Vulkan_RenderPass.hpp"

bool
Patache::Engine::CreateRenderPass (
    const Patache::SwapChainInfo & SwapChainInfo)
{
  vk::AttachmentDescription ColorAttachmentDescriptionInfo{
    .format         = SwapChainInfo.ImageColorFormat,
    .samples        = vk::SampleCountFlagBits::e1,
    .loadOp         = vk::AttachmentLoadOp::eClear,
    .storeOp        = vk::AttachmentStoreOp::eStore,
    .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
    .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
    .initialLayout  = vk::ImageLayout::eUndefined,
    .finalLayout    = vk::ImageLayout::ePresentSrcKHR
  };

  vk::AttachmentReference ColorAttachmentRef{
    .layout = vk::ImageLayout::eColorAttachmentOptimal
  };

  vk::SubpassDescription SubpassDescriptionInfo{
    .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &ColorAttachmentRef
  };

  vk::RenderPassCreateInfo RenderPassCreateInfo{
    .attachmentCount = 1,
    .pAttachments    = &ColorAttachmentDescriptionInfo,
    .subpassCount    = 1,
    .pSubpasses      = &SubpassDescriptionInfo,
    .dependencyCount = 0
  };

  vk::Result Result = Vulkan.Device.createRenderPass (
      &RenderPassCreateInfo, nullptr, &Vulkan.RenderPass);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Render Pass", Result);

      return false;
    }
  else
    return true;
}
