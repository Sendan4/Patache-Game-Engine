#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#include <future>

#include <fast_io.h>
#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"
