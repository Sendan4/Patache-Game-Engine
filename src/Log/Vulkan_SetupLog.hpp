#ifndef VULKAN_SETUPLOG_HPP
#define VULKAN_SETUPLOG_HPP

namespace Patache
{
void VulkanCheck (const char * const, const VkResult &);
void VulkanList (const char * const List[], const uint32_t &, const char * const);
}

#endif
