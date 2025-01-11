#include "Window.hpp"

void
Patata::Engine::EngineImpl::CreateGameWindow (const char * Title)
{
  std::string PatataWindowTitle;

  if (Title == nullptr)
    {
#if PATATA_GAME_NAME == 1
      PatataWindowTitle = PATATA_GAME_NAME;
#else
      PatataWindowTitle = "Patata Engine";
#endif

      if constexpr (PATATA_DEBUG)
        PatataWindowTitle += " (Debug / Development)";
    }
  else
    {
      PatataWindowTitle = Title;

      if constexpr (PATATA_DEBUG)
        PatataWindowTitle += " (Debug / Development)";
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

  std::future<void> Log
      = std::async (std::launch::async, Patata::Log::WindowLog, GameWindow
#if PATATA_DEBUG == 1
                    ,
                    &PatataEngineInfo
#endif
      );
}

#if defined(USE_ICON)
#include <SDL_syswm.h>

void
Patata::Engine::EngineImpl::SetWindowIcon (void)
{
  SDL_Surface * Icon = SDL_LoadBMP (PATATA_GAME_ICON_FILE);

  if (Icon == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "Icon cannot be loaded");

      return;
    }
  else
    SDL_SetWindowIcon (GameWindow, Icon);

  SDL_FreeSurface (Icon);
  Icon = nullptr;
}
#endif
