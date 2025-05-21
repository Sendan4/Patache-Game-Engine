#include <future>
#include <functional>
#include <cstring>

#if defined(__linux__)
#include <wayland-client.h>

// Wayland Protocols
#include <xdg-shell.h>
#include <xdg-decoration-unstable-v1.h>

#include <SDL3/SDL.h>
#include <fast_io.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "Log.hpp"
#include "CstringWrapped.hpp"

bool CreateWaylandWindow (const std::uint32_t &, const std::uint32_t &,
                          const char * const, Patache::Engine * const);

// xdg_wm_base
static void
PingToWindowManangerBase (void * Data, xdg_wm_base * WindowManangerBase,
                          std::uint32_t Serial)
{
  static_cast<void> (Data);
  xdg_wm_base_pong (WindowManangerBase, Serial);
}

static const struct xdg_wm_base_listener WindowManangerBaseListener
    = { .ping = PingToWindowManangerBase };

// xdg_surface
static void
DesktopStyleUserInterfaceConfigure (void *        Data,
                                    xdg_surface * DesktopStyleUserInterface,
                                    std::uint32_t Serial)
{
  static_cast<void> (Data);

  xdg_surface_ack_configure (DesktopStyleUserInterface, Serial);
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  if (Engine->ResizingPending)
    {
      Engine->Resize = true;

      SDL_SetWindowSize (Engine->GameWindow,
                         Engine->Vulkan.SwapChainExtent.width,
                         Engine->Vulkan.SwapChainExtent.height);
    }
}

static const struct xdg_surface_listener DesktopStyleUserInterfaceListener
    = { .configure = DesktopStyleUserInterfaceConfigure };

// Seat
// For now i not using Seat Inputs
// cast to void to silence GNU Compiler
// This function is not used, but is here to silence GNU Compiler
static void
PointerEnter (void * data, wl_pointer * wl_pointer, uint serial,
              wl_surface * surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  static_cast<void> (data);
  static_cast<void> (wl_pointer);
  static_cast<void> (serial);
  static_cast<void> (surface);
  static_cast<void> (surface_x);
  static_cast<void> (surface_y);
}

// This function is not used, but is here to silence GNU Compiler
static void
PointerLeave (void * data, wl_pointer * wl_pointer, uint serial,
              wl_surface * surface)
{
  static_cast<void> (data);
  static_cast<void> (wl_pointer);
  static_cast<void> (serial);
  static_cast<void> (surface);
}

// This function is not used, but is here to silence GNU Compiler
static void
PointerMotion (void * data, wl_pointer * wl_pointer, uint time,
               wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  static_cast<void> (data);
  static_cast<void> (wl_pointer);
  static_cast<void> (time);
  static_cast<void> (surface_x);
  static_cast<void> (surface_y);
}

// This function will be use, but is here to silence GNU Compiler
static void
PointerPressButton (void * data, wl_pointer * pointer, std::uint32_t serial,
                    std::uint32_t time, std::uint32_t button,
                    std::uint32_t state)
{
  static_cast<void> (data);
  static_cast<void> (pointer);
  static_cast<void> (serial);
  static_cast<void> (time);
  static_cast<void> (button);
  static_cast<void> (state);
}

// This function is not used, but is here to silence GNU Compiler
static void
PointerAxis (void * data, wl_pointer * wl_pointer, uint time, uint axis,
             wl_fixed_t value)
{
  static_cast<void> (data);
  static_cast<void> (wl_pointer);
  static_cast<void> (time);
  static_cast<void> (axis);
  static_cast<void> (value);
}

// This function is not used, but is here to silence GNU Compiler
static void
Frame (void * Data, wl_pointer * Pointer)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisSource (void * Data, wl_pointer * Pointer, std::uint32_t AxisSource)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
  static_cast<void> (AxisSource);
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisStop (void * Data, wl_pointer * Pointer, std::uint32_t Time,
          std::uint32_t Axis)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
  static_cast<void> (Time);
  static_cast<void> (Axis);
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisDiscrete (void * Data, wl_pointer * Pointer, std::uint32_t Axis,
              std::int32_t Discrete)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
  static_cast<void> (Axis);
  static_cast<void> (Discrete);
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisValue120 (void * Data, wl_pointer * Pointer, std::uint32_t Axis,
              std::int32_t Axis120)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
  static_cast<void> (Axis);
  static_cast<void> (Axis120);
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisRelativeDirection (void * Data, wl_pointer * Pointer, std::uint32_t Axis,
                       std::uint32_t RelativeDirection)
{
  static_cast<void> (Data);
  static_cast<void> (Pointer);
  static_cast<void> (Axis);
  static_cast<void> (RelativeDirection);
}

static const struct wl_pointer_listener PointerListener = {
  .enter                   = PointerEnter,
  .leave                   = PointerLeave,
  .motion                  = PointerMotion,
  .button                  = PointerPressButton,
  .axis                    = PointerAxis,
  .frame                   = Frame,
  .axis_source             = AxisSource,
  .axis_stop               = AxisStop,
  .axis_discrete           = AxisDiscrete,
  .axis_value120           = AxisValue120,
  .axis_relative_direction = AxisRelativeDirection,
};

static void
GetInputCapabilities (void * Data, wl_seat * Input, std::uint32_t Capabilities)
{
  // Using pointer input for client side window decorations
  if (Capabilities & WL_SEAT_CAPABILITY_POINTER)
    wl_pointer_add_listener (wl_seat_get_pointer (Input), &PointerListener,
                             Data);
}

// This function is not used, but is here to silence GNU Compiler
static void
Name (void * Data, wl_seat * Input, const char * Name)
{
  static_cast<void> (Data);
  static_cast<void> (Input);
  static_cast<void> (Name);
}

static const struct wl_seat_listener InputListener
    = { .capabilities = GetInputCapabilities, .name = Name };

// Registry
// Get Wayland handles from protocols and core
static void
AddObject (void * Data, wl_registry * Registry, std::uint32_t Name,
           const char * Interface, std::uint32_t Version)
{
  static_cast<void> (Version);
  Patache::WaylandWindow & WaylandWindow
      = *static_cast<Patache::WaylandWindow *> (Data);

  if (strcmp (Interface, wl_compositor_interface.name) == 0)
    {
      WaylandWindow.Compositor = static_cast<wl_compositor *> (
          wl_registry_bind (Registry, Name, &wl_compositor_interface, 1));
    }
  else if (strcmp (Interface, xdg_wm_base_interface.name) == 0)
    {
      WaylandWindow.WindowManangerBase = static_cast<xdg_wm_base *> (
          wl_registry_bind (Registry, Name, &xdg_wm_base_interface, 1));

      xdg_wm_base_add_listener (WaylandWindow.WindowManangerBase,
                                &WindowManangerBaseListener, nullptr);
    }
  else if (strcmp (Interface, wl_seat_interface.name) == 0)
    {
      WaylandWindow.Input = static_cast<wl_seat *> (
          wl_registry_bind (Registry, Name, &wl_seat_interface, 1));

      wl_seat_add_listener (WaylandWindow.Input, &InputListener, Data);
    }
  else if (strcmp (Interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
      WaylandWindow.DecorationMananger
          = static_cast<zxdg_decoration_manager_v1 *> (wl_registry_bind (
              Registry, Name, &zxdg_decoration_manager_v1_interface, 1));
    }
}

// This function is not used, but is here to silence GNU Compiler
static void
RemoveObject (void * Data, wl_registry * Registry, std::uint32_t Name)
{
  static_cast<void> (Data);
  static_cast<void> (Registry);
  static_cast<void> (Name);
}

static const struct wl_registry_listener RegistryListener
    = { .global = AddObject, .global_remove = RemoveObject };

// xdg_toplevel
static void
GetWindowSize (void * Data, xdg_toplevel * DesktopWindow, std::int32_t Width,
               std::int32_t Height, wl_array * States)
{
  static_cast<void> (DesktopWindow);
  static_cast<void> (States);

  // Resize Vulkan SwapChain
  if (Width != 0 && Height != 0)
    {
      Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);
      Engine->ResizingPending  = true;
      Engine->Vulkan.SwapChainExtent.width  = Width;
      Engine->Vulkan.SwapChainExtent.height = Height;
    }
}

static void
CloseWindow (void * Data, xdg_toplevel * DesktopWindow)
{
  static_cast<void> (Data);
  static_cast<void> (DesktopWindow);

  SDL_Event event{};
  event.type = SDL_EVENT_QUIT;
  SDL_PushEvent (&event);
}

// This function is not used, but is here to silence GNU Compiler
static void
ConfigureBounds (void * Data, xdg_toplevel * DesktopWindow, std::int32_t Width,
                 std::int32_t Height)
{
  static_cast<void> (Data);
  static_cast<void> (DesktopWindow);
  static_cast<void> (Width);
  static_cast<void> (Height);
}

// This function is not used, but is here to silence GNU Compiler
static void
WindowManangerCapabilities (void * Data, xdg_toplevel * DesktopWindow,
                            wl_array * States)
{
  static_cast<void> (Data);
  static_cast<void> (DesktopWindow);
  static_cast<void> (States);
}

static const struct xdg_toplevel_listener DesktopWindowListener
    = { .configure        = GetWindowSize,
        .close            = CloseWindow,
        .configure_bounds = ConfigureBounds,
        .wm_capabilities  = WindowManangerCapabilities };
#endif
