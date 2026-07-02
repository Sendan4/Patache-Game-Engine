#include <cstdint>
#include <functional>
#include <cstdio>
#if PATACHE_DEBUG == 1
  #include <chrono>
#endif

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
  #include <imgui.h>
  #include <imgui_impl_sdl3.h>
  #include <imgui_impl_vulkan.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Vulkan_SetupLog.hpp"

namespace Patache
{
static constexpr std::uint8_t sErrorTextSize{ 128U };
static constexpr std::uint8_t sErrorTextSizeExtraNull{ 129U };
}

#if PATACHE_LINUX_OR_UNIX
// Input.hpp
extern bool resize;
extern bool resizingPending;
// WaylandWindow.hpp
extern std::uint8_t scaleInt;
extern bool         scalePending;
#endif

#include "Vulkan_Swapchain.hpp"
