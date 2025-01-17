#include <future>

#include <SDL.h>
#include <SDL_vulkan.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "Log.hpp"

#define PATATA_ENGINE_VERSION_VK                                              \
  VK_MAKE_API_VERSION (0, PATATA_ENGINE_VERSION_MAYOR,                        \
                       PATATA_ENGINE_VERSION_MINOR,                           \
                       PATATA_ENGINE_VERSION_PATCH)
