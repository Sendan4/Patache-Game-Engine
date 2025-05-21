#include <future>

#include <fast_io.h>
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#endif
#if defined(__linux__)
#include <wayland-client.h>

// Wayland Protocols
#include <xdg-shell.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"

#if !defined(__linux__)
bool SDLCALL HandleResize (void * userdata, SDL_Event * event);
#endif
