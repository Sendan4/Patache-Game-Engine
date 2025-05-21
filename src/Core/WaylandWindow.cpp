#include "WaylandWindow.hpp"

bool
CreateWaylandWindow (const std::uint32_t & width, const std::uint32_t & height,
                     const char * const      WindowTitle,
                     Patache::Engine * const Engine)
{
  wl_registry * Registry = nullptr;

  // Display Connect
  Engine->WaylandWindow.Display = wl_display_connect (nullptr);

  if (Engine->WaylandWindow.Display == nullptr)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Wayland",
          "Cannot connect to wayland display. Use a wayland session",
          std::cref (Engine->configuration));

      return false;
    }

  // Display Get Registry
  Registry = wl_display_get_registry (Engine->WaylandWindow.Display);

  if (Registry == nullptr)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Wayland",
          "Cannot get a registry from a display. Use a wayland session",
          std::cref (Engine->configuration));

      return false;
    }

  wl_registry_add_listener (Registry, &RegistryListener,
                            &Engine->WaylandWindow);

  wl_display_roundtrip (Engine->WaylandWindow.Display);
  wl_registry_destroy (Registry);

  // Create Wayland Surface
  Engine->WaylandWindow.Surface
      = wl_compositor_create_surface (Engine->WaylandWindow.Compositor);

  if (Engine->WaylandWindow.Surface == nullptr)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Wayland",
          "The compositor cannot create a surface. Use a wayland session",
          std::cref (Engine->configuration));

      return false;
    }

  // Get Shell Surface
  Engine->WaylandWindow.DesktopStyleUserInterface
      = xdg_wm_base_get_xdg_surface (Engine->WaylandWindow.WindowManangerBase,
                                     Engine->WaylandWindow.Surface);

  if (Engine->WaylandWindow.DesktopStyleUserInterface == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Wayland",
                        "Cannot get xdg_surface. Use a wayland session",
                        std::cref (Engine->configuration));

      return false;
    }

  xdg_surface_add_listener (Engine->WaylandWindow.DesktopStyleUserInterface,
                            &DesktopStyleUserInterfaceListener, Engine);

  // Get xdg_toplevel
  Engine->WaylandWindow.DesktopWindow = xdg_surface_get_toplevel (
      Engine->WaylandWindow.DesktopStyleUserInterface);

  if (Engine->WaylandWindow.DesktopWindow == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Wayland",
                        "Cannot get xdg_toplevel. Use a wayland session",
                        std::cref (Engine->configuration));

      return false;
    }

  Engine->Vulkan.SwapChainExtent.width  = width;
  Engine->Vulkan.SwapChainExtent.height = height;

  xdg_toplevel_set_min_size (Engine->WaylandWindow.DesktopWindow, 640, 360);

  // Callback for Window events form the desktop
  xdg_toplevel_add_listener (Engine->WaylandWindow.DesktopWindow,
                             &DesktopWindowListener, Engine);

  xdg_toplevel_set_title (Engine->WaylandWindow.DesktopWindow, WindowTitle);
  xdg_toplevel_set_app_id (Engine->WaylandWindow.DesktopWindow, WindowTitle);

  if (Engine->WaylandWindow.DecorationMananger != nullptr)
    {
      Engine->WaylandWindow.Decoration
          = zxdg_decoration_manager_v1_get_toplevel_decoration (
              Engine->WaylandWindow.DecorationMananger,
              Engine->WaylandWindow.DesktopWindow);

      zxdg_toplevel_decoration_v1_set_mode (
          Engine->WaylandWindow.Decoration,
          ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    }

  wl_surface_commit (Engine->WaylandWindow.Surface);
  wl_display_roundtrip (Engine->WaylandWindow.Display);
  wl_surface_commit (Engine->WaylandWindow.Surface);

  // SDL Import Surface
  SDL_PropertiesID WindowProperties = SDL_CreateProperties ();
  SDL_SetPointerProperty (SDL_GetGlobalProperties (),
                          SDL_PROP_GLOBAL_VIDEO_WAYLAND_WL_DISPLAY_POINTER,
                          Engine->WaylandWindow.Display);
  SDL_SetPointerProperty (WindowProperties,
                          SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER,
                          Engine->WaylandWindow.Surface);
  SDL_SetBooleanProperty (WindowProperties,
                          SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);
  SDL_SetNumberProperty (WindowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,
                         width);
  SDL_SetNumberProperty (WindowProperties,
                         SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
  SDL_SetBooleanProperty (WindowProperties,
                          SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
  SDL_SetBooleanProperty (
      WindowProperties,
      SDL_PROP_WINDOW_CREATE_WAYLAND_CREATE_EGL_WINDOW_BOOLEAN, false);
  SDL_SetBooleanProperty (
      WindowProperties,
      SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN, true);
  SDL_SetBooleanProperty (
      WindowProperties,
      SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, true);
  SDL_SetBooleanProperty (WindowProperties,
                          SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN,
                          true);

  Engine->GameWindow = SDL_CreateWindowWithProperties (WindowProperties);

  if (Engine->GameWindow == nullptr)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - SDL & Wayland",
          "SDL Cannot import external surfaces. Use a wayland session",
          std::cref (Engine->configuration));

      return false;
    }

  return true;
}
