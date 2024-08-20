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
          if (State[SDL_SCANCODE_LALT] || State[SDL_SCANCODE_RALT])
            {
                if (Event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                  {
                    if (!PatataFullScreen)
                      {
                        PatataFullScreen = true;
                        SDL_SetWindowFullscreen (GameWindow, SDL_WINDOW_FULLSCREEN);
                      }
                    else
                      {
                        PatataFullScreen = false;
                        SDL_SetWindowFullscreen (GameWindow, 0);
                      }
                  }
            }
          break;
      }

      if (Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_RESIZED) {
        WindowResized = true;
      }

#if defined(DEBUG)
  // ImGui_ImplSDL2_ProcessEvent (&Event);
#endif
}
