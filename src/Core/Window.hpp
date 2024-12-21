#include <future>
#include <string>

#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#include <fast_io.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
// Begin Dependencies of PatataEngineImpl.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
// End Dependencies of PatataEngineImpl.hpp
#include "PatataEngineImpl.hpp"
#include "Log.hpp"

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
