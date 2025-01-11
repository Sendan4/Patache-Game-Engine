#include <future>

#include <fast_io.h>
#if PATATA_DEBUG == 1
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
// Begin Dependencies of PatataEngineImpl.hpp
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "Log.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
// End Dependencies of PatataEngineImpl.hpp
#include "PatataEngineImpl.hpp"
