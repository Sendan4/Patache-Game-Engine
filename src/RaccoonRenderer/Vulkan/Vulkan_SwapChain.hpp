#include <future>

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#if defined(DEBUG)
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif

// Patata Engine
#include "ColorTerminal.hpp"
// PatataEngineImpl.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "Log.hpp"
