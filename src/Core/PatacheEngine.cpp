#if PATACHE_DEBUG == 1 && !defined(_WIN64)
#include <cstdlib>
#endif
#include <future>
#include <cstring>

#if defined(_WIN64)
#include <windows.h>
#endif
#include <SDL3/SDL.h>
#include <fast_io.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "CstringWrapped.hpp"
#if PATACHE_DEBUG == 1
#include "CstdlibWrapped.hpp"
#endif
#include "Log.hpp"
// Icon
#if PATACHE_DEBUG == 1
#include "PatacheDebugIcon.hpp"
#else
#include "PatacheReleaseIcon.hpp" 
#endif

// VVL Path macros
#if defined(PATACHE_FIND_VVL_IN_THE_CURRENT_PATH)
#define PATACHE_VVL_PATH SDL_GetBasePath ()
#endif
#if defined(PATACHE_FIND_VVL_FROM_SDK)
#define PATACHE_VVL_PATH PATACHE_VVL_SDK_PATH
#endif

// Function Prototipes
// Event Filter for Window Resize
bool SDLCALL HandleResize (void * userdata, SDL_Event * event);

bool LoadConfiguration (Patache::Config &);

#if defined(__linux__)
bool CreateWaylandWindow (SDL_Window *);
#endif

bool RaccoonRendererInit (Patache::Engine *,
                          const Patache::EngineCreateInfo &);

void RaccoonRendererClose (Patache::VulkanBackend &);

bool
Patache::Engine::Init (const Patache::EngineCreateInfo & Info)
{
  {
    std::future<void> PatacheStartLogInfo = std::async (
        std::launch::async, Patache::Log::StartPatacheLogInfo);
  }

  if (!LoadConfiguration (configuration))
    return false;

#if defined(PATACHE_VVL_PATH)
  if (PATACHE_SETENV ("VK_LAYER_PATH", PATACHE_VVL_PATH)
      != PATACHE_SETENV_SUCCESS)
    std::future<void> Err = std::async (
        std::launch::async, Patache::Log::ErrorMessage,
        "Cannot set enviroment varible VK_LAYER_PATH");
#endif // PATACHE_VVL_PATH

  // SDL Subsystems
  if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - SDL2", SDL_GetError (),
                        configuration);

      return false;
    }

  {
    // Window Title
#if PATACHE_DEBUG == 1
    char WindowTitle[64]{ 0 };

    if (Info.windowTitle != nullptr)
      {
        PATACHE_STRNCPY (WindowTitle, Info.windowTitle, 63);
      }
    else if (Info.gameName != nullptr)
      {
        PATACHE_STRNCPY (WindowTitle, Info.gameName, 63);
      }
    else
      {
        PATACHE_STRNCPY (WindowTitle, PATACHE_ENGINE_NAME, 63);
      }

    PATACHE_STRNCAT (WindowTitle, " (Debug / Development)", 63);

#else  // PATACHE_DEBUG
    const char * WindowTitle
        = (Info.windowTitle == nullptr) ? Info.gameName : Info.windowTitle;

    if (WindowTitle == nullptr)
      WindowTitle = PATACHE_ENGINE_NAME;
#endif // PATACHE_DEBUG

    // Window Initial Size
    std::uint32_t w = 0, h = 0;

    // Displays
    int                           DisplaysCount = 0;
    SDL_DisplayID *               DID = SDL_GetDisplays (&DisplaysCount);
    const SDL_DisplayMode * const CurrentMode
        = SDL_GetCurrentDisplayMode (*DID);

    // Initial Resolution
    if (CurrentMode != nullptr)
      {
        w = static_cast<std::uint32_t> (CurrentMode->w * 0.64);
        h = static_cast<std::uint32_t> (CurrentMode->h * 0.64);
      }
    else
      {
        std::future<void> Err = std::async (
            std::launch::async, Patache::Log::ErrorMessage,
            "can't get the current resolution. starting with 480p "
                       "(1.78)");

        w = 854;
        h = 480;
      }

    // Create Window
    GameWindow = SDL_CreateWindow (WindowTitle, w, h,
                                   SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
                                       | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (GameWindow == nullptr)
      {
        std::future<void> Err = std::async (
            std::launch::async, Patache::Log::FatalErrorMessage,
            "Window cannot be created", SDL_GetError (),
            configuration);

        return false;
      }

    SDL_SetWindowMinimumSize (GameWindow, 640, 360);

#if PATACHE_DEBUG == 1
    engineInfo.WindowCreationFlags = SDL_GetWindowFlags (GameWindow);
#endif
  }

  // Set Window Icon
  {
    SDL_Surface * WindowIcon = nullptr;

    if (Info.windowIconPath != nullptr)
      {
        char Path[512]{ 0 };
        PATACHE_STRNCPY (Path, SDL_GetBasePath (), 511);
        PATACHE_STRNCAT (Path, "/", 511);
        PATACHE_STRNCAT (Path, Info.windowIconPath, 511);

        WindowIcon = SDL_LoadBMP (Path);
      }
    else
      WindowIcon = SDL_CreateSurfaceFrom (
          Patache::Icon::Width, Patache::Icon::Height,
          SDL_PIXELFORMAT_BGRA8888, (void *)Patache::Icon::Data,
          Patache::Icon::Pitch);

    if (WindowIcon == nullptr)
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::ErrorMessage,
          "Icon cannot be loaded");
    else
      SDL_SetWindowIcon (GameWindow, WindowIcon);

    SDL_DestroySurface (WindowIcon);
    WindowIcon = nullptr;
  }

  SDL_SetEventFilter (HandleResize, &WindowResized);

  if (!RaccoonRendererInit (this, Info))
    return false;

  return true;
}

Patache::Engine::Engine (const Patache::EngineCreateInfo & Info, bool * Err)
{
  if (Err != nullptr)
    *Err = Init (Info);
  else
    Init (Info);
}

#if PATACHE_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#endif

Patache::Engine::~Engine (void)
{
  RaccoonRendererClose (Vulkan);

#if PATACHE_DEBUG == 1
  // Imgui
  ImGuiIO & io = ImGui::GetIO ();

  if (io.BackendPlatformName != nullptr)
    ImGui_ImplSDL3_Shutdown ();

  if (ImGui::GetCurrentContext () != nullptr)
    ImGui::DestroyContext ();

  // Struct / Patache Engine Info
  if (engineInfo.ppVkLayers != nullptr)
    {
      delete[] engineInfo.ppVkLayers;
      engineInfo.ppVkLayers = nullptr;
    }

  if (engineInfo.ppVkInstanceExtensions != nullptr)
    {
      delete[] engineInfo.ppVkInstanceExtensions;
      engineInfo.ppVkInstanceExtensions    = nullptr;
      engineInfo.VkInstanceExtensionsCount = 0;
    }

  if (engineInfo.ppVkDeviceExtensions != nullptr)
    {
      delete[] engineInfo.ppVkDeviceExtensions;
      engineInfo.ppVkDeviceExtensions    = nullptr;
      engineInfo.VkDeviceExtensionsCount = 0;
    }

  engineInfo.VkDeviceVendorId    = 0;
  engineInfo.WindowCreationFlags = 0;
#endif

  // SDL
  SDL_DestroyWindow (GameWindow);
  SDL_QuitSubSystem (SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Quit ();
}
