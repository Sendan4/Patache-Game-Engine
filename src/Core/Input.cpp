#include "Input.hpp"

void
Patache::Engine::HandleEvent (const SDL_Event & rEvent)
{
  const bool * const pKeyState = SDL_GetKeyboardState (nullptr);

  switch (rEvent.type)
    {
    case SDL_EVENT_KEY_DOWN:

      // (begin) ALT + RETURN Hotkey for toggle full screen
      if (pKeyState[SDL_SCANCODE_RALT] || pKeyState[SDL_SCANCODE_LALT])
        {
          if (rEvent.key.key == SDLK_RETURN)
            {
              if (!isFullScreen)
                {
                  isFullScreen = true;

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
                  // Wayland Client Side Decoration
                  if (waylandWindow.pDecorationMananger == nullptr)
                    {
                      for (std::uint8_t i = 0; i < 3; ++i)
                        wl_subsurface_destroy (waylandWindow.pButtonSubSurface[i]);

                      wl_subsurface_destroy (waylandWindow.pMainBarSubSurface);

                      if (!isMaximized)
                        {
                          for (std::uint8_t i = 0; i < PATACHE_BORDER_CSD_SIZE; ++i)
                            wl_subsurface_destroy (waylandWindow.pBorderSubSurface[i]);
                        }
                    }

                  xdg_toplevel_set_fullscreen (waylandWindow.pDesktopWindow, nullptr);
#else
                  // SDL_Window
                  int                           displaysCount = 0;
                  SDL_DisplayID *               pDID          = SDL_GetDisplays (&displaysCount);
                  const SDL_DisplayMode * const pDesktopMode  = SDL_GetDesktopDisplayMode (*pDID);

                  if (pDesktopMode != nullptr)
                    {
                      if (!SDL_SetWindowFullscreenMode (pGameWindow, pDesktopMode))
                        std::future<void> err
                            = std::async (std::launch::async, Patache::ErrorMessage,
                                          "Unable to apply full screen resolution");
                    }
                  else
                    {
                      std::future<void> err
                          = std::async (std::launch::async, Patache::ErrorMessage,
                                        "Could not obtain Desktop Display Mode mode or "
                                        "Display ID");
                    }

                  if (!SDL_SetWindowFullscreen (pGameWindow, true))
                    std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                                        "Could not switch to full screen mode");
#endif
                }
              else
                {
#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
                  // Wayland Client Side Decoration
                  xdg_toplevel_unset_fullscreen (waylandWindow.pDesktopWindow);

                  if (waylandWindow.pDecorationMananger == nullptr)
                    {
                      waylandWindow.pMainBarSubSurface = wl_subcompositor_get_subsurface (
                          waylandWindow.pSubCompositor, waylandWindow.pMainBarSurface,
                          waylandWindow.pSurface);

                      // Buttons
                      for (std::uint8_t i = 0; i < 3; ++i)
                        {
                          waylandWindow.pButtonSubSurface[i] = wl_subcompositor_get_subsurface (
                              waylandWindow.pSubCompositor, waylandWindow.pButtonSurface[i],
                              waylandWindow.pMainBarSurface);
                        }

                      // Border Window
                      if (!isMaximized)
                        {
                          for (std::uint8_t i = 0; i < PATACHE_BORDER_CSD_SIZE; ++i)
                            {
                              waylandWindow.pBorderSubSurface[i] = wl_subcompositor_get_subsurface (
                                  waylandWindow.pSubCompositor, waylandWindow.pBorderSurface[i],
                                  waylandWindow.pSurface);
                            }
                        }
                    }
#else
                  // SDL_Window
                  if (!SDL_SetWindowFullscreen (pGameWindow, false))
                    std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                                        "could not switch to window mode");
#endif
                  isFullScreen = false;
                }
            }
        }
        // (end) ALT + RETURN Hotkey for toggle full screen

#if PATACHE_DEBUG == 1
      // (begin) CTRL + P hotkey for toggle the debug ui
      if (pKeyState[SDL_SCANCODE_LCTRL] || pKeyState[SDL_SCANCODE_RCTRL])
        if (rEvent.key.key == SDLK_P)
          debugInfo.showMainMenuBar ^= true;
      // (end) CTRL + P hotkey for toggle the debug ui
#endif
      break;
    }

#if PATACHE_DEBUG == 1
  ImGui_ImplSDL3_ProcessEvent (&rEvent);
#endif
}

#if defined(_WIN64)
// SDL Event Filter for Window Resize
bool SDLCALL
HandleResize (void * pUserdata, SDL_Event * pEvent)
{
  if (pEvent->window.type == SDL_EVENT_WINDOW_RESIZED
      || pEvent->window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
      || pEvent->window.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN
      || pEvent->window.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN)
    *static_cast<bool *> (pUserdata) = true;

  return true;
}
#endif
