#include <future>

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include "PatacheEngine/VmaUsage.hpp"
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
#include "Message.hpp"

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
extern bool isMaximized;
#else
bool SDLCALL HandleResize (void *, SDL_Event *);
#endif

bool isFullScreen = false;
bool resize       = false;
