#include "Window.hpp"

void
Patata::Engine::EngineImpl::CreateGameWindow (const std::string & Title,
                                              const uint32_t &    WindowWidth,
                                              const uint32_t &    WindowHeight)
{
  std::string PatataWindowTitle;

  if (!Title.empty ())
#if defined(DEBUG)
    PatataWindowTitle = Title + " (Debug / Development)";
#else
    PatataWindowTitle = Title;
#endif
  else
    {
#if defined(PATATA_GAME_NAME)
#if defined(DEBUG)
      PatataWindowTitle = PATATA_GAME_NAME;
      PatataWindowTitle += " (Debug / Development)";
#else
      PatataWindowTitle = PATATA_GAME_NAME;
#endif
#else
#if defined(DEBUG)
      PatataWindowTitle = "Patata Engine (Debug / Development)";
#else
      PatataWindowTitle = "Patata Engine";
#endif
#endif
    }

  GameWindow
      = SDL_CreateWindow (PatataWindowTitle.c_str (), SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
                              | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

  if (!GameWindow)
    {
      Patata::Log::FatalErrorMessage ("Window cannot be created",
                                      SDL_GetError (), Config);
      return;
    }

  Patata::Log::WindowLog (GameWindow);
}

#if defined(USE_ICON)
#include <SDL_syswm.h>

void
Patata::Engine::EngineImpl::SetWindowIcon (void)
{
#if defined(_WIN64)
  SDL_Surface * Icon = SDL_LoadBMP (PATATA_GAME_ICON_FILE);

  if (Icon == nullptr)
    Patata::Log::ErrorMessage ("Icon cannot be loaded");
  else
    SDL_SetWindowIcon (GameWindow, Icon);

  SDL_FreeSurface (Icon);
#else
  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (GameWindow, &WindowInfo);

  if (WindowInfo.subsystem == SDL_SYSWM_WAYLAND)
    {
      Patata::Log::WarningMessage (
          "Dynamic icons are not supported in Wayland");
    }
  else
    {
      SDL_Surface * Icon = SDL_LoadBMP (PATATA_GAME_ICON_FILE);

      if (Icon == nullptr)
        Patata::Log::ErrorMessage ("Icon cannot be loaded");
      else
        SDL_SetWindowIcon (GameWindow, Icon);

      fast_io::io::perrln (std::string_view{ SDL_GetError () });
      SDL_FreeSurface (Icon);
    }
#endif
}
#endif
