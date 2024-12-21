#include <future>

#include <fast_io.h>
#if defined(DEBUG)
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
// PatataEngineImpl.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "PatataEngineImpl.hpp"
#include "Log.hpp"
