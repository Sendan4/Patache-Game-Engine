// Patata Engine
#include "Input.hpp"

void
Patata::Engine::HandleEvent (const SDL_Event & Event)
{
  static bool PatataFullScreen = false;

  const bool * const KeyState = SDL_GetKeyboardState (nullptr);

  switch (Event.type)
    {
    case SDL_EVENT_KEY_DOWN:
      // (begin) ALT + RETURN Hotkey for toggle full screen
      if (KeyState[SDL_SCANCODE_RALT] || KeyState[SDL_SCANCODE_LALT])
        {
          if (Event.key.key == SDLK_RETURN)
            {
              if (!PatataFullScreen)
                {
                  PatataFullScreen              = true;
                  int             DisplaysCount = 0;
                  SDL_DisplayID * DID = SDL_GetDisplays (&DisplaysCount);
                  const SDL_DisplayMode * const DesktopMode
                      = SDL_GetDesktopDisplayMode (*DID);

                  if (DesktopMode != nullptr)
                    {
                      if (!SDL_SetWindowFullscreenMode (GameWindow,
                                                        DesktopMode))
                        std::future<void> Err = std::async (
                            std::launch::async, Patata::Log::ErrorMessage,
                            "Unable to apply full screen resolution");
                    }
                  else
                    {
                      std::future<void> Err = std::async (
                          std::launch::async, Patata::Log::ErrorMessage,
                          "Could not obtain Desktop Display Mode mode or "
                          "Display ID");
                    }

                  if (!SDL_SetWindowFullscreen (GameWindow,
                                                SDL_WINDOW_FULLSCREEN))
                    std::future<void> Err = std::async (
                        std::launch::async, Patata::Log::ErrorMessage,
                        "Could not switch to full screen mode");
                }
              else
                {
                  PatataFullScreen = false;
                  if (!SDL_SetWindowFullscreen (GameWindow, 0))
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
      if (KeyState[SDL_SCANCODE_LCTRL] || KeyState[SDL_SCANCODE_RCTRL])
        if (Event.key.key == SDLK_P)
          engineInfo.ShowMainMenuBar ^= true;
#endif
      break;
    }

  // resize event
  if (Event.window.type == SDL_EVENT_WINDOW_RESIZED
      || Event.window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    WindowResized = true;

#if PATATA_DEBUG == 1
  ImGui_ImplSDL3_ProcessEvent (&Event);
#endif
}
