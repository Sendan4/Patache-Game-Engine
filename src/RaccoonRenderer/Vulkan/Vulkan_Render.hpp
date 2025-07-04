#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <future>
#include <functional>
#include <cstdio>

#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#endif
#include <fast_io.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "StartLogInfo.hpp"
#include "Log.hpp"

void RecreateSwapChain (Patache::Engine * const);

#define PATACHE_ERROR_TEXT_SIZE 128

#if defined(__linux__)
extern bool Resize;
extern bool ResizingPending;
#endif
