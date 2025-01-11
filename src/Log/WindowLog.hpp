#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif

#include <fast_io.h>
#if PATATA_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif
#include <SDL_syswm.h>
#include <SDL.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "ColorTerminal.hpp"
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "Log.hpp"
