#include <future>

#include <vulkan/vulkan.hpp>
#include <SDL.h>
#if defined(DEBUG)
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif

// Patata Engine
// PatataEngineImpl.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "Log.hpp"
