// Patata Engine
#include "Input.hpp"

void
Patata::Engine::EngineImpl::HandleEvent (SDL_Event & Event)
{
  static bool PatataFullScreen = false;

  const uint8_t * State = SDL_GetKeyboardState (nullptr);

  switch (Event.type)
    {
    case SDL_KEYDOWN:
      // (begin) ALT + RETURN Hotkey for toggle full screen
      if (State[SDL_SCANCODE_LALT] || State[SDL_SCANCODE_RALT])
        {
          if (Event.key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
              if (!PatataFullScreen)
                {
                  PatataFullScreen = true;
                  SDL_DisplayMode DesktopMode;

                  if (SDL_GetDesktopDisplayMode (0, &DesktopMode) == 0)
                    {
                      if (SDL_SetWindowDisplayMode (GameWindow, &DesktopMode)
                          != 0)
                        {
                          std::future<void> Err = std::async (
                              std::launch::async, Patata::Log::ErrorMessage,
                              "Unable to apply full screen resolution");
                        }
                    }
                  else
                    {
                      std::future<void> Err = std::async (
                          std::launch::async, Patata::Log::ErrorMessage,
                          "Could not obtain desktop display mode");
                    }

                  if (SDL_SetWindowFullscreen (GameWindow,
                                               SDL_WINDOW_FULLSCREEN)
                      != 0)
                    {
                      std::future<void> Err = std::async (
                          std::launch::async, Patata::Log::ErrorMessage,
                          "Could not switch to full screen mode");
                    }
                }
              else
                {
                  PatataFullScreen = false;
                  if (SDL_SetWindowFullscreen (GameWindow, 0) != 0)
                    {
                      std::future<void> Err = std::async (
                          std::launch::async, Patata::Log::ErrorMessage,
                          "could not switch to window mode");
                    }
                }
            }
        }
        // (end) ALT + RETURN Hotkey for toggle full screen

        // (begin) ctrl + p hotkey for toggle the menubar of debug ui
#if PATATA_DEBUG == 1
      if (State[SDL_SCANCODE_LCTRL] || State[SDL_SCANCODE_RCTRL])
        {
          if (Event.key.keysym.scancode == SDL_SCANCODE_P)
            {
              if (PatataEngineInfo.ShowMainMenuBar)
                PatataEngineInfo.ShowMainMenuBar = false;
              else
                PatataEngineInfo.ShowMainMenuBar = true;
            }
        }
#endif

      // (end) ALT + RETURN Hotkey for toggle full screen
      break;

    case SDL_WINDOWEVENT:
      // resize event
      if (Event.window.event == SDL_WINDOWEVENT_RESIZED
          || Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        WindowResized = true;
      break;
    }

#if PATATA_DEBUG == 1
  ImGui_ImplSDL2_ProcessEvent (&Event);
#endif
}
