#if defined(__GNUC__) || defined(__MINGW64__)
  #include <cxxabi.h>
#endif
#include <future>
#include <functional>
#include <cstdio>
#include <cmath>
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
#include "LinuxAndUnixMacro.h"

// Vulkan_Swapchain.cpp
void RecreateSwapchain (Patache::Engine * const);

#define PATACHE_ERROR_TEXT_SIZE 128

#if PATACHE_LINUX_OR_UNIX
// Input.hpp
extern bool resize;
extern bool resizingPending;
// WaylandWindow.hpp
extern std::uint8_t scaleInt;
extern bool         scalePending;
#endif
