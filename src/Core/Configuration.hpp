#include <string>
#include <future>

#if PATATA_DEBUG == 1
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
#if PATATA_DEBUG == 1
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

#if PATATA_GAME_NAME != 0
#define GAME_CONFIG_FILE_NAME PATATA_GAME_NAME ".yaml"
#else
#define GAME_CONFIG_FILE_NAME "patata.yaml"
#endif
