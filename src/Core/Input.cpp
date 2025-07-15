#include "Input.hpp"

void
Patache::Engine::HandleEvent (const SDL_Event & Event)
{
  const bool * const KeyState = SDL_GetKeyboardState (nullptr);

  switch (Event.type)
    {
    case SDL_EVENT_KEY_DOWN:

      // (begin) ALT + RETURN Hotkey for toggle full screen
      if (KeyState[SDL_SCANCODE_RALT] || KeyState[SDL_SCANCODE_LALT])
        {
          if (Event.key.key == SDLK_RETURN)
            {
              if (!IsFullScreen)
                {
                  IsFullScreen = true;

#if defined(__linux__)
                  if (WaylandWindow.DecorationMananger == nullptr)
                    {
                      for (std::uint8_t i = 0; i < 3; ++i)
                        {
                          wl_subsurface_destroy (
                              WaylandWindow.ButtonSubSurface[i]);
                        }

                      wl_subsurface_destroy (WaylandWindow.MainBarSubSurface);

                      if (!IsMaximized)
                        {
                          for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_SIZE;
                               ++i)
                            wl_subsurface_destroy (
                                WaylandWindow.BorderSubSurface[i]);
                        }
                    }

                  xdg_toplevel_set_fullscreen (WaylandWindow.DesktopWindow,
                                               nullptr);
#else
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

                  if (!SDL_SetWindowFullscreen (GameWindow, IsFullScreen))
                    std::future<void> Err = std::async (
                        std::launch::async, Patache::Log::ErrorMessage,
                        "Could not switch to full screen mode");
#endif
                }
              else
                {
#if defined(__linux__)
                  xdg_toplevel_unset_fullscreen (WaylandWindow.DesktopWindow);

                  if (WaylandWindow.DecorationMananger == nullptr)
                    {
                      WaylandWindow.MainBarSubSurface
                          = wl_subcompositor_get_subsurface (
                              WaylandWindow.SubCompositor,
                              WaylandWindow.MainBarSurface,
                              WaylandWindow.Surface);

                      // Buttons
                      for (std::uint8_t i = 0; i < 3; ++i)
                        {
                          WaylandWindow.ButtonSubSurface[i]
                              = wl_subcompositor_get_subsurface (
                                  WaylandWindow.SubCompositor,
                                  WaylandWindow
                                      .ButtonSurface[i], // surface turned into
                                                         // a sub-surface
                                  WaylandWindow
                                      .MainBarSurface); // the parent surface
                        }

                      // Border Window
                      if (!IsMaximized)
                        {
                          for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_SIZE;
                               ++i)
                            {
                              WaylandWindow.BorderSubSurface[i]
                                  = wl_subcompositor_get_subsurface (
                                      WaylandWindow.SubCompositor,
                                      WaylandWindow
                                          .BorderSurface[i], // surface turned
                                                             // into a
                                                             // sub-surface
                                      WaylandWindow
                                          .Surface); // the parent surface
                            }
                        }
                    }
#else
                  if (!SDL_SetWindowFullscreen (GameWindow, IsFullScreen))
                    std::future<void> Err = std::async (
                        std::launch::async, Patache::Log::ErrorMessage,
                        "could not switch to window mode");
#endif
                  IsFullScreen = false;
                }
            }
        }
        // (end) ALT + RETURN Hotkey for toggle full screen

#if PATACHE_DEBUG == 1
      // (begin) ctrl + p hotkey for toggle the debug ui
      if (KeyState[SDL_SCANCODE_LCTRL] || KeyState[SDL_SCANCODE_RCTRL])
        if (Event.key.key == SDLK_P)
          engineInfo.ShowMainMenuBar ^= true;
      // (end) ctrl + p hotkey for toggle the debug ui
#endif
      break;
    }

#if PATACHE_DEBUG == 1
  ImGui_ImplSDL3_ProcessEvent (&Event);
#endif
}

#if !defined(__linux__)
// Event Filter for Window Resize
bool SDLCALL
HandleResize (void * userdata, SDL_Event * event)
{
  // resize event
  if (event->window.type == SDL_EVENT_WINDOW_RESIZED
      || event->window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
      || event->window.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN
      || event->window.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN)
    *static_cast<bool *> (userdata) = true;

  return true;
}
#endif
