// OS
#if __linux__ && !__ANDROID__
#define PATATA_OS "Linux"
#elif _WIN64 || _WIN32
#define PATATA_OS "Windows"
#endif

// Compiler
#if __GNUC__
#if __clang__
#define PATATA_COMPILER "Clang"
#elif __MINGW64__
#define PATATA_COMPILER "MinGW"
#else
#define PATATA_COMPILER "GCC"
#endif
#elif _MSC_VER <= 1929
#define PATATA_COMPILER "Old Microsoft Visual C++ (MSVC)"
#elif _MSC_VER >= 1930
#define PATATA_COMPILER "Microsoft Visual C++ (MSVC)"
#endif

#include <fast_io.h>

// Patata Engine
#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include "Log.hpp"
#include "TerminalColors.hpp"
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include <yaml-cpp/yaml.h>

#if defined(_WIN64)
#include <windows.h>
#endif
