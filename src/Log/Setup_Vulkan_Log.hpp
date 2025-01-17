#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#include <SDL.h>
#include <fast_io.h>
#if defined(_WIN64)
#include <windows.h>
#endif

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
