#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#include <fast_io.h>
#include <yaml-cpp/yaml.h>
#include <vulkan/vulkan.hpp>

#include "Log.hpp"
#include "TerminalColors.hpp"
#include "RaccoonRenderer.hpp"
