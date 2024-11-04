#include <future>

#include <fast_io.h>
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#if defined(DEBUG)
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#endif

#include "Log.hpp"
#include "RaccoonRenderer.hpp"
#include "TerminalColors.hpp"
