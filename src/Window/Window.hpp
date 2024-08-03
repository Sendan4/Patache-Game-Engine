#include <SDL.h>
#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <yaml-cpp/yaml.h>

#include "Log.hpp"
#include "PatataEngineImpl.hpp"
#include "TerminalColors.hpp"

#if defined(USE_ICON)
#if defined(PATATA_GAME_NAME)
#define PATATA_GAME_ICON_FILE PATATA_GAME_NAME ".bmp"
#else
#if defined(DEBUG)
#define PATATA_GAME_ICON_FILE "icon-debug.bmp"
#else
#define PATATA_GAME_ICON_FILE "icon-release.bmp"
#endif
#endif
#endif
