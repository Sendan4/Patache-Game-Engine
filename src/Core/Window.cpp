#include "Window.hpp"

void
Patata::Engine::EngineImpl::CreateGameWindow (const char * Title)
{
  std::string PatataWindowTitle;

  if (Title == nullptr)
    {
#if defined(PATATA_GAME_NAME)
      PatataWindowTitle = PATATA_GAME_NAME;
#else
      PatataWindowTitle = "Patata Engine";
#endif
#if defined(DEBUG)
      PatataWindowTitle += " (Debug / Development)";
#endif
    }
  else
    {
      PatataWindowTitle = Title;
#if defined(DEBUG)
      PatataWindowTitle += " (Debug / Development)";
#endif
    }

  uint32_t        w = 0, h = 0;
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
          "can't get the current resolution. starting with 480p (1.78)");

      w = 854;
      h = 480;
    }

  GameWindow
      = SDL_CreateWindow (PatataWindowTitle.c_str (), SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED, w, h,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
                              | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

  if (!GameWindow)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Window cannot be created", SDL_GetError (), Configuration);

      return;
    }

  SDL_SetWindowMinimumSize (GameWindow, 640, 360);

#if defined(DEBUG)
  std::future<void> Log
      = std::async (std::launch::async, Patata::Log::WindowLog, GameWindow,
                    &PatataEngineInfo);
#else
  std::future<void> Log
      = std::async (std::launch::async, Patata::Log::WindowLog, GameWindow);
#endif
}

#if defined(USE_ICON)
#include <SDL_syswm.h>

void
Patata::Engine::EngineImpl::SetWindowIcon (void)
{
#if defined(_WIN64) // Only Windows
  SDL_Surface * Icon = SDL_LoadBMP (PATATA_GAME_ICON_FILE);

  if (Icon == nullptr)
    std::future<void> Err
        = std::async (std::launch::async, Patata::Log::ErrorMessage,
                      "Icon cannot be loaded");
  else
    SDL_SetWindowIcon (GameWindow, Icon);

  SDL_FreeSurface (Icon);
#else // Linux
  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (GameWindow, &WindowInfo);

  if (WindowInfo.subsystem != SDL_SYSWM_WAYLAND)
    {
      SDL_Surface * Icon = SDL_LoadBMP (PATATA_GAME_ICON_FILE);

      if (Icon == nullptr)
        std::future<void> Err
            = std::async (std::launch::async, Patata::Log::ErrorMessage,
                          "Icon cannot be loaded");
      else
        SDL_SetWindowIcon (GameWindow, Icon);

      fast_io::io::perrln (std::string_view{ SDL_GetError () });
      SDL_FreeSurface (Icon);
    }
#endif
}
#endif
