#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#include <future>
#include <limits>

#include <fast_io.h>
#include <yaml-cpp/yaml.h>
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include "Log.hpp"
#include "TerminalColors.hpp"
#include "RaccoonRenderer.hpp"
