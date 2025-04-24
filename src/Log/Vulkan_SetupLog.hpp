#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#include <SDL3/SDL.h>
#include <fast_io.h>
#if defined(_WIN64)
#include <windows.h>
#endif
#if PATATA_DEBUG == 1
#include <imgui.h>
#endif

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"
