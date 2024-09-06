#include <future>

#if defined(DEBUG)
#include <imgui_impl_sdl2.h>
#endif
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngineImpl.hpp"
#include "TerminalColors.hpp"
#include "Log.hpp"
