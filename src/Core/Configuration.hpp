#include <string>
#include <future>

#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#include <fast_io.h>
#include <fast_io_device.h>
#include <ryml.hpp>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
// Begin Dependencies of PatataEngineImpl.hpp
#include "StructConfig.hpp"
#include "Log.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
// End Dependencies of PatataEngineImpl.hpp
#include "PatataEngineImpl.hpp"

#if defined(PATATA_GAME_NAME)
#define GAME_CONFIG_FILE_NAME PATATA_GAME_NAME ".yaml"
#else
#define GAME_CONFIG_FILE_NAME "patata.yaml"
#endif
