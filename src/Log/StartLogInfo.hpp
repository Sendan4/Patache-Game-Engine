// OS
#if __linux__ && !__ANDROID__
#define PATACHE_OS "Linux"
#elif _WIN64 || _WIN32
#define PATACHE_OS "Windows"
#endif

// Compiler
#if __GNUC__
#if __clang__
#define PATACHE_COMPILER "Clang"
#elif __MINGW64__
#define PATACHE_COMPILER "Minimalist GNU for Windows (MinGW-w64)"
#else
#define PATACHE_COMPILER "GNU Compiler Collection (GCC)"
#endif
#elif _MSC_VER <= 1929
#define PATACHE_COMPILER "Old Microsoft Visual C++ (MSVC)"
#elif _MSC_VER >= 1930
#define PATACHE_COMPILER "Microsoft Visual C++ (MSVC)"
#endif

#include <fast_io.h>
#include <SDL3/SDL.h>
#if defined(_WIN64)
#include <windows.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"
