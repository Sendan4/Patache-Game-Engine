#include <future>

#include <fast_io.h>
#if defined(_WIN64)
#include <windows.h>
#endif
#include <SDL.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "Log.hpp"
#include "patata_icon.hpp"

// VVL Path macros
#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
#define PATATA_VVL_PATH SDL_GetBasePath ()
#endif
#if defined(PATATA_FIND_VVL_FROM_SDK)
#define PATATA_VVL_PATH PATATA_VVL_SDK_PATH
#endif

#if defined(_WIN64)
#define PATATA_SETENV(VARIABLE, VALUE) SetEnvironmentVariable (VARIABLE, VALUE)
#define PATATA_SETENV_SUCCESS          1
#else
#define PATATA_SETENV(VARIABLE, VALUE) setenv (VARIABLE, VALUE, 1)
#define PATATA_SETENV_SUCCESS          0
#endif

bool
Patata::Engine::Init (const Patata::EngineCreateInfo & Info)
{
  {
    std::future<void> PatataStartLogInfo = std::async (
        std::launch::async, Patata::Log::StartPatataLogInfo, Info);
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
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - SDL2", SDL_GetError (), configuration);
      fast_io::io::println ("Test");
      return false;
    }

  // Create Window
  {
    // Window Title
#if PATATA_DEBUG == 1
    std::string WindowTitle;
#define PATATA_WINDOW_TITLE_STR WindowTitle.c_str ()

    if (Info.windowTitle == nullptr)
      if (Info.gameName != nullptr)
        WindowTitle = Info.gameName;
      else
        WindowTitle = "Patata Engine";
    else
      WindowTitle = Info.windowTitle;

    WindowTitle.append (" (Debug / Development)");
#else
    const char * WindowTitle
        = (Info.windowTitle == nullptr) ? Info.gameName : Info.windowTitle;
#define PATATA_WINDOW_TITLE_STR WindowTitle

    if (WindowTitle == nullptr)
      WindowTitle = "Patata Engine";
#endif

    // Window Initial Size
    uint32_t w = 0, h = 0;

    SDL_DisplayMode CurrentMode;

    if (SDL_GetCurrentDisplayMode (0, &CurrentMode) == 0)
      {
        w = static_cast<uint32_t> (CurrentMode.w * 0.64);
        h = static_cast<uint32_t> (CurrentMode.h * 0.64);
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
    GameWindow = SDL_CreateWindow (
        PATATA_WINDOW_TITLE_STR, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, w, h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
            | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!GameWindow)
      {
        std::future<void> Err = std::async (
            std::launch::async, Patata::Log::FatalErrorMessage,
            "Window cannot be created", SDL_GetError (), configuration);

        return false;
      }

    SDL_SetWindowMinimumSize (GameWindow, 640, 360);

    std::future<void> Log
        = std::async (std::launch::async, &Patata::Engine::WindowLog, this);
  }

  // Set Window Icon
  {
    SDL_Surface * WindowIcon = nullptr;

    if (Info.windowIconPath != nullptr)
      {
        std::string Path{ SDL_GetBasePath () };
        Path.append ("/");
        Path.append (Info.windowIconPath);

        WindowIcon = SDL_LoadBMP (Path.c_str ());
      }
    else
      WindowIcon = SDL_CreateRGBSurfaceWithFormatFrom (
          (void *)Patata::Icon::Data, Patata::Icon::Height,
          Patata::Icon::Width, Patata::Icon::Bitdepth, Patata::Icon::Pitch,
          SDL_PIXELFORMAT_BGRA8888);

    if (WindowIcon == nullptr)
      {
        std::future<void> Err
            = std::async (std::launch::async, Patata::Log::ErrorMessage,
                          "Icon cannot be loaded");

        return false;
      }
    else
      SDL_SetWindowIcon (GameWindow, WindowIcon);

    SDL_FreeSurface (WindowIcon);
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
}

#if PATATA_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#endif

Patata::Engine::~Engine (void)
{
  RaccoonRendererClose ();

#if PATATA_DEBUG == 1
  // Imgui
  ImGuiIO & io = ImGui::GetIO ();

  if (io.BackendPlatformName != nullptr)
    ImGui_ImplSDL2_Shutdown ();

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
  SDL_QuitSubSystem (SDL_INIT_VIDEO | SDL_INIT_EVENTS
                     | SDL_INIT_GAMECONTROLLER);
  SDL_Quit ();
}
