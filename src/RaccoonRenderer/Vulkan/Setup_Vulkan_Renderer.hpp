#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <tuple>
#include <future>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include <fast_io.h>
#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#endif

#include "Config.hpp"
#if defined(DEBUG)
#include "EngineInfo.hpp"
#endif
#include "Log.hpp"
#include "RaccoonRenderer.hpp"
#include "TerminalColors.hpp"
