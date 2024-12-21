#include <future>

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#if defined(DEBUG)
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif

#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "PatataEngine/StructClearColor.hpp"
#include "StructConfig.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "Log.hpp"

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
