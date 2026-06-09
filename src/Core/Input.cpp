#include "Input.hpp"

void
Patache::Engine::HandleEvent (const SDL_Event & rEvent)
{
  const bool * const pKeyState{ SDL_GetKeyboardState (nullptr) };

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
#if PATACHE_LINUX_OR_UNIX
                  // Wayland Client Side Decoration
                  if (waylandWindow.pDecorationMananger == nullptr
                      && waylandWindow.pSubCompositor != nullptr)
                    {
                      if (!isMaximized)
                        {
                          // From window
                          for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
                            {
                              wl_subsurface_destroy (waylandWindow.pBorderSubSurface[i]);
                              wl_subsurface_destroy (waylandWindow.pShadowSubSurface[i]);
                            }

                          for (std::uint8_t i{ 0U }; i < 3U; ++i)
                            {
                              wl_subsurface_destroy (waylandWindow.pButtonSubSurface[i]);
                            }

                          wl_subsurface_destroy (waylandWindow.pMainBarSubSurface);

                          xdg_toplevel_set_fullscreen (waylandWindow.pDesktopWindow, nullptr);
                          isFullScreen = true;
                        }
                      else
                        {
                          // Por el exigente de weston debo desmaximizar primero y luego debo
                          // solicitar la pantalla completa, no se pueden hacer las 2 cosas en la
                          // misma funcion. la solicitud para la pantalla completa esta dentro de
                          // un callback del protocolo wayland
                          xdg_toplevel_unset_maximized (waylandWindow.pDesktopWindow);

                          for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
                            {
                              waylandWindow.pBorderSubSurface[i] = wl_subcompositor_get_subsurface (
                                  waylandWindow.pSubCompositor, waylandWindow.pBorderSurface[i],
                                  waylandWindow.pSurface);

                              waylandWindow.pShadowSubSurface[i] = wl_subcompositor_get_subsurface (
                                  waylandWindow.pSubCompositor, waylandWindow.pShadowSurface[i],
                                  waylandWindow.pSurface);
                            }

                          isMaximized       = false;
                          waitForFullscreen = true;
                        }
                    }
#else
                  // SDL_Window
                  int                           displaysCount{ 0U };
                  SDL_DisplayID *               pDID{ SDL_GetDisplays (&displaysCount) };
                  const SDL_DisplayMode * const pDesktopMode{ SDL_GetDesktopDisplayMode (*pDID) };

                  if (pDesktopMode != nullptr)
                    {
                      if (!SDL_SetWindowFullscreenMode (pGameWindow, pDesktopMode))
                        {
                          Patache::ErrorMessage ("Unable to apply full screen resolution");
                        }
                    }
                  else
                    {
                      Patache::ErrorMessage ("Could not obtain Desktop Display Mode mode or "
                                             "Display ID");
                    }

                  if (!SDL_SetWindowFullscreen (pGameWindow, true))
                    {
                      Patache::ErrorMessage ("Could not switch to full screen mode");
                    }
#endif
                }
              else
                {
#if PATACHE_LINUX_OR_UNIX
                  // Wayland Client Side Decoration
                  returnFromFullscreen = true;

                  xdg_toplevel_unset_fullscreen (waylandWindow.pDesktopWindow);

                  if (waylandWindow.pDecorationMananger == nullptr
                      && waylandWindow.pSubCompositor != nullptr)
                    {
                      // Buttons
                      for (std::uint8_t i{ 0U }; i < 3U; ++i)
                        {
                          waylandWindow.pButtonSubSurface[i] = wl_subcompositor_get_subsurface (
                              waylandWindow.pSubCompositor, waylandWindow.pButtonSurface[i],
                              waylandWindow.pMainBarSurface);
                        }

                      waylandWindow.pMainBarSubSurface = wl_subcompositor_get_subsurface (
                          waylandWindow.pSubCompositor, waylandWindow.pMainBarSurface,
                          waylandWindow.pSurface);

                      if (!isMaximized)
                        {
                          for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
                            {
                              waylandWindow.pBorderSubSurface[i] = wl_subcompositor_get_subsurface (
                                  waylandWindow.pSubCompositor, waylandWindow.pBorderSurface[i],
                                  waylandWindow.pSurface);

                              waylandWindow.pShadowSubSurface[i] = wl_subcompositor_get_subsurface (
                                  waylandWindow.pSubCompositor, waylandWindow.pShadowSurface[i],
                                  waylandWindow.pSurface);
                            }
                        }
                    }
#else
                  // SDL_Window
                  if (!SDL_SetWindowFullscreen (pGameWindow, false))
                    {
                      Patache::ErrorMessage ("could not switch to window mode");
                    }
#endif
                  isFullScreen = false;
                }
            }
        }
        // (end) ALT + RETURN Hotkey for toggle full screen

#if PATACHE_DEBUG == 1
      // (begin) CTRL + P hotkey for toggle the debug ui
      if (pKeyState[SDL_SCANCODE_LCTRL] || pKeyState[SDL_SCANCODE_RCTRL])
        {
          if (rEvent.key.key == SDLK_P)
            {
              debugInfo.showMainMenuBar ^= true;
            }
        }
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
