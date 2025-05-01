#include "Input.hpp"

void
Patache::Engine::HandleEvent (const SDL_Event & Event)
{
  static bool        FullScreen = false;
  const bool * const KeyState   = SDL_GetKeyboardState (nullptr);

  switch (Event.type)
    {
    case SDL_EVENT_KEY_DOWN:

      // (begin) ALT + RETURN Hotkey for toggle full screen
      if (KeyState[SDL_SCANCODE_RALT] || KeyState[SDL_SCANCODE_LALT])
        {
          if (Event.key.key == SDLK_RETURN)
            {
              if (!FullScreen)
                {
                  FullScreen = true;

                  int             DisplaysCount = 0;
                  SDL_DisplayID * DID = SDL_GetDisplays (&DisplaysCount);
                  const SDL_DisplayMode * const DesktopMode
                      = SDL_GetDesktopDisplayMode (*DID);

                  if (DesktopMode != nullptr)
                    {
                      if (!SDL_SetWindowFullscreenMode (GameWindow,
                                                        DesktopMode))
                        std::future<void> Err = std::async (
                            std::launch::async, Patache::Log::ErrorMessage,
                            "Unable to apply full screen resolution");
                    }
                  else
                    std::future<void> Err = std::async (
                        std::launch::async, Patache::Log::ErrorMessage,
                        "Could not obtain Desktop Display Mode mode or "
                        "Display ID");

                  if (!SDL_SetWindowFullscreen (GameWindow, FullScreen))
                    std::future<void> Err = std::async (
                        std::launch::async, Patache::Log::ErrorMessage,
                        "Could not switch to full screen mode");
                }
              else
                {
                  FullScreen = false;

                  if (!SDL_SetWindowFullscreen (GameWindow, FullScreen))
                    std::future<void> Err = std::async (
                        std::launch::async, Patache::Log::ErrorMessage,
                        "could not switch to window mode");
                }
            }
        }
        // (end) ALT + RETURN Hotkey for toggle full screen

        // (begin) ctrl + p hotkey for toggle the debug ui
#if PATACHE_DEBUG == 1
      if (KeyState[SDL_SCANCODE_LCTRL] || KeyState[SDL_SCANCODE_RCTRL])
        if (Event.key.key == SDLK_P)
          engineInfo.ShowMainMenuBar ^= true;
#endif
      // (end) ctrl + p hotkey for toggle the debug ui
      break;
    }

#if PATACHE_DEBUG == 1
  ImGui_ImplSDL3_ProcessEvent (&Event);
#endif
}

// Event Filter for Window Resize
bool SDLCALL
HandleResize (void * userdata, SDL_Event * event)
{
  // resize event
  if (event->window.type == SDL_EVENT_WINDOW_RESIZED
      || event->window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
      || event->window.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN
      || event->window.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN)
    *reinterpret_cast<bool *> (userdata) = true;
  return true;
}
