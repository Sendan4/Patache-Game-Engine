#if PATATA_DEBUG == 1 && !defined(_WIN64)
#include <cstdlib>
#endif
#include <future>
#include <cstring>

#if defined(_WIN64)
#include <windows.h>
#endif
#include <SDL3/SDL.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "CstringWrapped.hpp"
#if PATATA_DEBUG == 1
#include "CstdlibWrapped.hpp"
#endif
#include "Log.hpp"
#include "patata_icon.hpp"

// VVL Path macros
#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
#define PATATA_VVL_PATH SDL_GetBasePath ()
#endif
#if defined(PATATA_FIND_VVL_FROM_SDK)
#define PATATA_VVL_PATH PATATA_VVL_SDK_PATH
#endif

bool
Patata::Engine::Init (const Patata::EngineCreateInfo & Info)
{
  {
    std::future<void> PatataStartLogInfo
        = std::async (std::launch::async, Patata::Log::StartPatataLogInfo);
  }

  if (!LoadConfiguration ())
    return false;

#if defined(PATATA_VVL_PATH)
  if (PATATA_SETENV ("VK_LAYER_PATH", PATATA_VVL_PATH)
      != PATATA_SETENV_SUCCESS)
    std::future<void> Err
        = std::async (std::launch::async, Patata::Log::ErrorMessage,
                      "Cannot set enviroment varible VK_LAYER_PATH");
#endif // PATATA_VVL_PATH

  // SDL Subsystems
  if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - SDL2", SDL_GetError (), configuration);

      return false;
    }

  // Create Window
  {
    // Window Title
#if PATATA_DEBUG == 1
    char WindowTitle[64]{ 0 };

    if (Info.windowTitle != nullptr)
      {
        PATATA_STRNCPY (WindowTitle, Info.windowTitle, 63);
      }
    else if (Info.gameName != nullptr)
      {
        PATATA_STRNCPY (WindowTitle, Info.gameName, 63);
      }
    else
      {
        PATATA_STRNCPY (WindowTitle, PATATA_ENGINE_NAME, 63);
      }

    PATATA_STRNCAT (WindowTitle, " (Debug / Development)", 63);

#else  // PATATA_DEBUG
    const char * WindowTitle
        = (Info.windowTitle == nullptr) ? Info.gameName : Info.windowTitle;

    if (WindowTitle == nullptr)
      WindowTitle = PATATA_ENGINE_NAME;
#endif // PATATA_DEBUG

    // Window Initial Size
    std::uint32_t w = 0, h = 0;

    int                           DisplaysCount = 0;
    SDL_DisplayID *               DID = SDL_GetDisplays (&DisplaysCount);
    const SDL_DisplayMode * const CurrentMode
        = SDL_GetCurrentDisplayMode (*DID);

    if (CurrentMode != nullptr)
      {
        w = static_cast<std::uint32_t> (CurrentMode->w * 0.64);
        h = static_cast<std::uint32_t> (CurrentMode->h * 0.64);
      }
    else
      {
        std::future<void> Err = std::async (
            std::launch::async, Patata::Log::ErrorMessage,
            "can't get the current resolution. starting with 480p "
            "(1.78)");

        w = 854;
        h = 480;
      }

    // Create Window
    GameWindow = SDL_CreateWindow (WindowTitle, w, h,
                                   SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
                                       | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!GameWindow)
      {
        std::future<void> Err = std::async (
            std::launch::async, Patata::Log::FatalErrorMessage,
            "Window cannot be created", SDL_GetError (), configuration);

        return false;
      }

    SDL_SetWindowMinimumSize (GameWindow, 640, 360);

#if PATATA_DEBUG == 1
    engineInfo.WindowCreationFlags = SDL_GetWindowFlags (GameWindow);
#endif
  }

  // Set Window Icon
  {
    SDL_Surface * WindowIcon = nullptr;

    if (Info.windowIconPath != nullptr)
      {
        char Path[512]{ 0 };
        PATATA_STRNCPY (Path, SDL_GetBasePath (), 511);
        PATATA_STRNCAT (Path, "/", 511);
        PATATA_STRNCAT (Path, Info.windowIconPath, 511);

        WindowIcon = SDL_LoadBMP (Path);
      }
    else
      WindowIcon = SDL_CreateSurfaceFrom (
          Patata::Icon::Width, Patata::Icon::Height, SDL_PIXELFORMAT_BGRA8888,
          (void *)Patata::Icon::Data, Patata::Icon::Pitch);

    if (WindowIcon == nullptr)
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "Icon cannot be loaded");
    else
      SDL_SetWindowIcon (GameWindow, WindowIcon);

    SDL_DestroySurface (WindowIcon);
    WindowIcon = nullptr;
  }

  if (!RaccoonRendererInit (Info))
    return false;

  return true;
}

Patata::Engine::Engine (const Patata::EngineCreateInfo & Info, bool * Err)
{
  if (Err != nullptr)
    *Err = Init (Info);
  else
    Init (Info);
}

#if PATATA_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#endif

Patata::Engine::~Engine (void)
{
  RaccoonRendererClose ();

#if PATATA_DEBUG == 1
  // Imgui
  ImGuiIO & io = ImGui::GetIO ();

  if (io.BackendPlatformName != nullptr)
    ImGui_ImplSDL3_Shutdown ();

  if (ImGui::GetCurrentContext () != nullptr)
    ImGui::DestroyContext ();

  // Struct / Patata Engine Info
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
