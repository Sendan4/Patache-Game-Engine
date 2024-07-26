// Patata Engine
#include "Input.hpp"

void
Patata::Engine::EngineImpl::HandleEvent (SDL_Event & Event)
{
  static bool PatataKeyPress   = false;
  static bool PatataFullScreen = false;

#if defined(DEBUG)
  // ImGui_ImplSDL2_ProcessEvent (&Event);
#endif

  switch (Event.type)
    {
    case SDL_KEYDOWN:
      if (!PatataKeyPress)
        {
          switch (Event.key.keysym.sym)
            {
            case SDLK_F11:
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
              break;
            }
          PatataKeyPress = true;
        }
      break;

    case SDL_KEYUP:
      if (PatataKeyPress)
        PatataKeyPress = false;
      break;
    }
}
