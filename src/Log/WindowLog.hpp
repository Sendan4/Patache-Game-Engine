#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif

#include <fast_io.h>
#include <SDL.h>
#include <SDL_syswm.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
