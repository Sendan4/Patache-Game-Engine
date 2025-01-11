#include <future>

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif

// Patata Engine
#include "ColorTerminal.hpp"
// PatataEngineImpl.hpp
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "Log.hpp"
