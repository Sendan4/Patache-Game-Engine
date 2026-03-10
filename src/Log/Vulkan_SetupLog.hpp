#pragma once

#include <fast_io.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"

namespace Patache
{
void VulkanCheck (const char * const, const VkResult &);
void VulkanList (const char * const List[], const uint32_t &, const char * const);
}
