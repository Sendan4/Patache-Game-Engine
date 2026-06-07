#ifndef VULKAN_SETUPRENDERER_HPP
#define VULKAN_SETUPRENDERER_HPP

namespace Patache
{
bool RaccoonRendererInit (Patache::Engine *, const Patache::EngineCreateInfo &);
void RaccoonRendererClose (Patache::VulkanBackend &);
}

#endif
