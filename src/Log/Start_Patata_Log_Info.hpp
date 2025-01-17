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
#define PATATA_COMPILER "Minimalist GNU for Windows (MinGW-w64)"
#else
#define PATATA_COMPILER "GNU Compiler Collection (GCC)"
#endif
#elif _MSC_VER <= 1929
#define PATATA_COMPILER "Old Microsoft Visual C++ (MSVC)"
#elif _MSC_VER >= 1930
#define PATATA_COMPILER "Microsoft Visual C++ (MSVC)"
#endif

#include <fast_io.h>
#include <SDL.h>
#if defined(_WIN64)
#include <windows.h>
#endif

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
