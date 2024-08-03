#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#include <future>

#include <fast_io.h>

#include "Log.hpp"
#include "RaccoonRenderer.hpp"

#include <SDL_vulkan.h>

#if defined(_WIN64)
#include <windows.h>
#endif

#include "TerminalColors.hpp"
#include "ExitLog.hpp"

#ifndef PATATA_GAME_NAME
#define PATATA_GAME_NAME "Unknown"
#endif
#ifndef PATATA_GAME_VERSION_MAYOR
#define PATATA_GAME_VERSION_MAYOR 0
#endif
#ifndef PATATA_GAME_VERSION_MINOR
#define PATATA_GAME_VERSION_MINOR 0
#endif
#ifndef PATATA_GAME_VERSION_PATCH
#define PATATA_GAME_VERSION_PATCH 0
#endif

#define PATATA_GAME_VERSION                                                   \
  VK_MAKE_VERSION (PATATA_GAME_VERSION_MAYOR, PATATA_GAME_VERSION_MINOR,      \
                   PATATA_GAME_VERSION_PATCH)
#define PATATA_ENGINE_VERSION_VK                                              \
  VK_MAKE_VERSION (PATATA_ENGINE_VERSION_MAYOR, PATATA_ENGINE_VERSION_MINOR,  \
                   PATATA_ENGINE_VERSION_PATCH)
