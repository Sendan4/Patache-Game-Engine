#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <future>
#include <cstdio>

#include <SDL.h>
#if PATATA_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#endif

#include "PatataEngine/PatataEngine.hpp"
#include "StartLogInfo.hpp"
#include "Log.hpp"

#define PATATA_ERROR_TEXT_SIZE 128
