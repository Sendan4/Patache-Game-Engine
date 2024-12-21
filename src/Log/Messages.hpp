#include <string>

#include <fast_io.h>
#if defined(DEBUG)
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#if defined (_WIN64)
#include <windows.h>
#endif

// Patata Engine
#include "ColorTerminal.hpp"
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "Log.hpp"
