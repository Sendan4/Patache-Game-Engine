#if defined(GAME_NAME)
#define GAME_CONFIG_FILE_NAME GAME_NAME ".yaml"
#endif

#include <fast_io.h>

#if defined(_WIN64)
#include <windows.h>
#endif

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include <yaml-cpp/yaml.h>

#include "Log.hpp"
#include "TerminalColors.hpp"
