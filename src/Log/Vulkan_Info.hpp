#if PATACHE_DEBUG == 1
  #include <cstdio>
  #include <future>
  #include <cstring>
#endif

#include <fast_io.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
  #include <imgui_impl_vulkan.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"
#include "CstringWrapped.hpp"

void VulkanInfo (Patache::Engine * const, const Patache::SwapchainInfo &);
