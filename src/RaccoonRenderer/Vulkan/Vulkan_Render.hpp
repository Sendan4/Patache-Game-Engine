#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <future>
#include <cstdio>

#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "StartLogInfo.hpp"
#include "Log.hpp"

#define PATACHE_ERROR_TEXT_SIZE 128
