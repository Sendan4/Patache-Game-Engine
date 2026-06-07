#include <cstring>

#if defined(_WIN64)
  #include <windows.h> // wincon.h
#endif
#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <fast_io.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "CstringWrapped.hpp"
#include "ColorTerminal.hpp"
#if PATACHE_DEBUG == 1
  #include "CstdlibWrapped.hpp"
#endif
#include "StartLogInfo.hpp"
#include "Message.hpp"
// Icon
#if PATACHE_DEBUG == 1
  #include "PatacheDebugIcon.hpp"
#else
  #include "PatacheReleaseIcon.hpp"
#endif

#if defined(_WIN64)
static HANDLE sTerminal = GetStdHandle (STD_OUTPUT_HANDLE);
static DWORD  sMode     = 0;
#endif

// VVL Path macros
#if defined(PATACHE_FIND_VVL_IN_THE_CURRENT_PATH)
  #define PATACHE_VVL_PATH SDL_GetBasePath ()
#endif
#if defined(PATACHE_FIND_VVL_FROM_SDK)
  #define PATACHE_VVL_PATH PATACHE_VVL_SDK_PATH
#endif

#include "Configuration.hpp"

#if PATACHE_LINUX_OR_UNIX
  #include "WaylandWindow.hpp"
#else
// Event Filter for Window Resize | Input.cpp Func
bool SDLCALL HandleResize (void *, SDL_Event *);
extern bool  resize;
#endif

#include "Vulkan_SetupRenderer.hpp"

bool
Patache::Engine::Init (const Patache::EngineCreateInfo & rInfo)
{
#if PATACHE_DEBUG == 1
  debugInfo.startTimer = std::chrono::high_resolution_clock::now ();
#endif

#if defined(_WIN64)
  GetConsoleMode (sTerminal, &sMode);
  SetConsoleMode (sTerminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | sMode);
#endif

  StartLogInfo ();

  if (!Patache::LoadConfiguration (configuration))
    return false;

#if defined(PATACHE_VVL_PATH)
  if (PATACHE_SETENV ("VK_LAYER_PATH", PATACHE_VVL_PATH) != PATACHE_SETENV_SUCCESS)
    {
      Patache::ErrorMessage ("Cannot set enviroment varible VK_LAYER_PATH");
    }
#endif

#if PATACHE_DEBUG == 1
  // Debug
  char windowTitle[64]{ 0 };

  if (rInfo.pWindowTitle != nullptr)
    {
      PATACHE_STRNCPY (windowTitle, rInfo.pWindowTitle, 63, 64);
    }
  else if (rInfo.pGameName != nullptr)
    {
      PATACHE_STRNCPY (windowTitle, rInfo.pGameName, 63, 64);
    }
  else
    {
      PATACHE_STRNCPY (windowTitle, PATACHE_ENGINE_NAME, 63, 64);
    }

  PATACHE_STRNCAT (windowTitle, " (Debug / Development)", 63, 64);

  #define PATACHE_WINDOW_TITLE windowTitle
#else
  // Release
  const char * pWindowTitle{ (rInfo.pWindowTitle != nullptr) ? rInfo.pWindowTitle
                                                             : rInfo.pGameName };

  if (pWindowTitle == nullptr)
    pWindowTitle = PATACHE_ENGINE_NAME;

  #define PATACHE_WINDOW_TITLE pWindowTitle
#endif

  // Init Window for linux wayland. do this before SDL init
#if PATACHE_LINUX_OR_UNIX
  if (!Patache::CreateWaylandWindow (854, 480, PATACHE_WINDOW_TITLE, this))
    return false;
#endif

  // Init SDL Subsystems
  if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
      Patache::FatalErrorMessage ("Patache Engine - SDL2", SDL_GetError (), configuration);

      return false;
    }

    // Init Window for other Systems
#if defined(_WIN64)
  {
    // Window Initial Size
    std::uint32_t width{ 0U }, height{ 0U };

    // Displays
    int                           displaysCount{ 0U };
    SDL_DisplayID *               dID{ SDL_GetDisplays (&displaysCount) };
    const SDL_DisplayMode * const pCurrentMode{ SDL_GetCurrentDisplayMode (*dID) };

    // Initial Resolution
    if (pCurrentMode != nullptr)
      {
        width  = static_cast<std::uint32_t> (pCurrentMode->w * 0.64F);
        height = static_cast<std::uint32_t> (pCurrentMode->h * 0.64F);
      }
    else
      {
        Patache::ErrorMessage ("can't get the current resolution. starting with 480p "
                               "(1.78)");

        width  = 854U;
        height = 480U;
      }

    // Create Window
    pGameWindow = SDL_CreateWindow (PATACHE_WINDOW_TITLE, width, height,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
                                        | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (pGameWindow == nullptr)
      {
        Patache::FatalErrorMessage ("Window cannot be created", SDL_GetError (), configuration);

        return false;
      }

    SDL_SetWindowMinimumSize (pGameWindow, 640, 360);
  }

  // Set Window Icon
  {
    SDL_Surface * pWindowIcon{ nullptr };

    if (rInfo.pWindowIconPath != nullptr)
      {
        char path[512U]{ 0 };
        PATACHE_STRNCPY (path, SDL_GetBasePath (), 511U, 512U);
        PATACHE_STRNCAT (path, "/", 511U, 512U);
        PATACHE_STRNCAT (path, rInfo.pWindowIconPath, 511U, 512U);

        pWindowIcon = SDL_LoadBMP (path);
      }
    else
      {
        pWindowIcon = SDL_CreateSurfaceFrom (Patache::Icon::Width, Patache::Icon::Height,
                                             SDL_PIXELFORMAT_BGRA8888, (void *)Patache::Icon::Data,
                                             Patache::Icon::Pitch);
      }

    if (pWindowIcon == nullptr)
      {
        Patache::ErrorMessage ("Icon cannot be loaded");
      }
    else
      {
        SDL_SetWindowIcon (pGameWindow, pWindowIcon);
      }

    SDL_DestroySurface (pWindowIcon);
    pWindowIcon = nullptr;
  }

  SDL_SetEventFilter (HandleResize, &resize);
#endif

#if PATACHE_DEBUG == 1
  debugInfo.windowCreationFlags = SDL_GetWindowFlags (pGameWindow);
#endif

  if (!Patache::RaccoonRendererInit (this, rInfo))
    return false;

#if PATACHE_DEBUG == 1
  debugInfo.startTime = std::chrono::duration<float, std::chrono::milliseconds::period> (
                            std::chrono::high_resolution_clock::now () - debugInfo.startTimer)
                            .count ();

  fast_io::println (PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE, PATACHE_ENGINE_NAME,
                    PATACHE_TERM_RESET, " : Start time : ", debugInfo.startTime, " milliseconds");
#endif

  return true;
}

Patache::Engine::Engine (const Patache::EngineCreateInfo & rInfo, bool * pErr)
{
  if (pErr != nullptr)
    *pErr = Init (rInfo);
  else
    Init (rInfo);
}

#if PATACHE_DEBUG == 1
  #include <imgui.h>
  #include <imgui_impl_sdl3.h>
#endif

Patache::Engine::~Engine (void)
{
  Patache::RaccoonRendererClose (vulkan);

#if PATACHE_DEBUG == 1
  // Imgui
  ImGuiIO * pIO{ nullptr };

  if (ImGui::GetCurrentContext () != nullptr)
    {
      pIO = &ImGui::GetIO ();

      if (pIO->BackendPlatformName != nullptr)
        ImGui_ImplSDL3_Shutdown ();

      ImGui::DestroyContext ();
    }

  // Debug Info
  if (debugInfo.ppLayersVK != nullptr)
    {
      std::free (debugInfo.ppLayersVK);
      debugInfo.ppLayersVK = nullptr;
    }

  if (debugInfo.ppInstanceExtensionsVK != nullptr)
    {
      std::free (debugInfo.ppInstanceExtensionsVK);
      debugInfo.ppInstanceExtensionsVK    = nullptr;
      debugInfo.instanceExtensionsCountVK = 0U;
    }

  if (debugInfo.ppDeviceExtensionsVK != nullptr)
    {
      std::free (debugInfo.ppDeviceExtensionsVK);
      debugInfo.ppDeviceExtensionsVK    = nullptr;
      debugInfo.deviceExtensionsCountVK = 0U;
    }

  if (debugInfo.ppVramMemoryDeviceSize != nullptr)
    {
      for (std::uint8_t i{ 0U }; i < vulkan.swapchainImageCount; ++i)
        {
          if (debugInfo.ppVramMemoryDeviceSize[i] != nullptr)
            {
              std::free (debugInfo.ppVramMemoryDeviceSize[i]);
            }
        }

      std::free (debugInfo.ppVramMemoryDeviceSize);
      debugInfo.ppVramMemoryDeviceSize = nullptr;
    }

  if (debugInfo.ppVramMemoryDeviceSizeUnit != nullptr)
    {
      for (std::uint8_t i{ 0U }; i < vulkan.swapchainImageCount; ++i)
        {
          if (debugInfo.ppVramMemoryDeviceSizeUnit[i] != nullptr)
            {
              std::free (debugInfo.ppVramMemoryDeviceSizeUnit[i]);
            }
        }

      std::free (debugInfo.ppVramMemoryDeviceSizeUnit);
      debugInfo.ppVramMemoryDeviceSizeUnit = nullptr;
    }

  if (debugInfo.ppVramMemoryDeviceHeap != nullptr)
    {
      for (std::uint8_t i{ 0U }; i < vulkan.swapchainImageCount; ++i)
        {
          if (debugInfo.ppVramMemoryDeviceHeap[i] != nullptr)
            {
              std::free (debugInfo.ppVramMemoryDeviceHeap[i]);
            }
        }

      std::free (debugInfo.ppVramMemoryDeviceHeap);
      debugInfo.ppVramMemoryDeviceHeap = nullptr;
    }

  if (debugInfo.ppVramMemoryDeviceType != nullptr)
    {
      for (std::uint8_t i{ 0U }; i < vulkan.swapchainImageCount; ++i)
        {
          if (debugInfo.ppVramMemoryDeviceType[i] != nullptr)
            {
              std::free (debugInfo.ppVramMemoryDeviceType[i]);
            }
        }

      std::free (debugInfo.ppVramMemoryDeviceType);
      debugInfo.ppVramMemoryDeviceType = nullptr;
    }

  debugInfo.deviceVendorIdVK    = 0U;
  debugInfo.windowCreationFlags = 0U;
#endif

  // SDL
  SDL_DestroyWindow (pGameWindow);

  // Wayland
#if PATACHE_LINUX_OR_UNIX
  if (waylandWindow.pCursorTheme != nullptr)

    {
      wl_cursor_theme_destroy (waylandWindow.pCursorTheme);
      waylandWindow.pCursorTheme = nullptr;
    }
#endif

  // SDL
  SDL_QuitSubSystem (SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Quit ();

#if defined(_WIN64)
  SetConsoleMode (sTerminal, sMode);
#endif
}
