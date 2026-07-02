#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

namespace Patache
{
bool CreateSwapchain (Patache::Engine * const, Patache::SwapchainInfo &);
void RecreateSwapchain (Patache::Engine * const);
}

#endif
