#pragma once
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#include <fast_io.h>
#include <vulkan/vulkan.hpp>

// Patache Engine
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"
#include "LogMacroUtils.hpp"

namespace Patache
{
void VulkanCheck (const char * const, const vk::Result &);
void VulkanList (const char * const List[], const uint32_t &, const char * const);
}
