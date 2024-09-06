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
                        SDL_DisplayMode DesktopMode;

                        if (SDL_GetDesktopDisplayMode (0, &DesktopMode) == 0)
                          {
                            if (SDL_SetWindowDisplayMode (GameWindow, &DesktopMode) != 0)
                              {
                                std::future<void> Err = std::async (
                                    std::launch::async,
                                    Patata::Log::ErrorMessage,
                                    "Unable to apply full screen resolution");
                              }
                            else
                              {
                                fast_io::io::println (
                                    #if defined(_WIN64)
                                    fast_io::out (),
                                    #endif
                                    PATATA_TERM_BOLD,
                                    "FullScreen Resolution : ",
                                    PATATA_TERM_RESET, DesktopMode.w, " x ", DesktopMode.h,
                                    " | ", DesktopMode.refresh_rate, " Hz");
                              }
                          }
                        else
                          {
                            std::future<void> Err = std::async (
                                std::launch::async, Patata::Log::ErrorMessage,
                                "Could not obtain desktop display mode");
                          }

                        if (SDL_SetWindowFullscreen (GameWindow, SDL_WINDOW_FULLSCREEN) != 0)
                          {
                            std::future<void> Err = std::async (
                                std::launch::async,
                                Patata::Log::ErrorMessage,
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
          break;
      }

      if (Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_RESIZED)
        WindowResized = true;

#if defined(DEBUG)
  // ImGui_ImplSDL2_ProcessEvent (&Event);
#endif
}
