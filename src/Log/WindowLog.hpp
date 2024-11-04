#include <cstring>
#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif

#include <SDL.h>
#include <SDL_syswm.h>
#include <fast_io.h>
#if defined(_WIN64)
#include <windows.h>
#endif
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "Log.hpp"
#include "TerminalColors.hpp"
#if defined(DEBUG)
#include "EngineInfo.hpp"
#endif
