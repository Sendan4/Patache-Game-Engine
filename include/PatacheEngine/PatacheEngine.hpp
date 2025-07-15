#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>
#if defined(__linux__)
#include <wayland-client.h>
#include <wayland-cursor.h>

// Wayland Protocols
#include <xdg-shell.h>
#include <xdg-decoration-unstable-v1.h>
#endif

#include "PatacheEngine/Libexport.h"
#include "PatacheEngine/Color.hpp"
#include "PatacheEngine/VulkanBackend.hpp"
#include "PatacheEngine/StructConfig.hpp"
#include "PatacheEngine/StructEngineInfo.hpp"

namespace Patache
{
// Versioning management in the style of Vulkan. // major.minor.patch.variant
constexpr uint32_t
MakeVersion (const uint8_t & Major, const uint16_t & Minor,
             const uint16_t & Patch, const uint8_t & Variant)
{
  return ((static_cast<uint32_t> (Variant)) << 29U
          | (static_cast<uint32_t> (Major)) << 22U
          | (static_cast<uint32_t> (Minor)) << 12U
          | static_cast<uint32_t> (Patch));
}

constexpr uint8_t
GetVersionMajor (const uint32_t & Major)
{
  // 6 bits
  return Major >> 22U & 0b1111111;
}

constexpr uint16_t
GetVersionMinor (const uint32_t & Minor)
{
  // 9 bits
  return Minor >> 12U & 0b1111111111;
}

constexpr uint16_t
GetVersionPatch (const uint32_t & Patch)
{
  // 11 bits
  return Patch & 0b111111111111;
}

constexpr uint8_t
GetVersionVariant (const uint32_t & Variant)
{
  // 5 bits
  return Variant >> 29U & 0b111111;
}

enum class ViewMode : bool
{
  TwoDimensions, // 2D
  TreeDimensions // 3D
};

// you can use nullptr to not use the options or not assign them at all.
struct EngineCreateInfo
{
  const char * const gameName = nullptr; // Name of the game // It is optional.
  const uint32_t     gameVersion = 0;
  // Vulkan version style: major.minor.patch.variant // It is
  // optional.
  const char * const windowTitle = nullptr;
  // Initial title of the window. // It is optional.
  const Patache::ViewMode viewMode = Patache::ViewMode::TwoDimensions;
  // Determines whether the project uses 2D or 3D graphics.
  // It is mandatory.
  const char * const windowIconPath = nullptr;
  // Initial icon of the window. // It is optional.
  // Use bitmap (.bmp) format/codec. // It is optional.
};

// Float32 format
struct ClearColor
{
  float r = 0.0f; // Red
  float g = 0.0f; // Green
  float b = 0.0f; // Blue
  float a = 1.0f; // Alpha
};

#if defined(__linux__)
struct WaylandWindow
{
  // Globals
  wl_display *    Display    = nullptr;
  wl_compositor * Compositor = nullptr;
  wl_seat *       Input      = nullptr;
  // Main Surface for graphics
  wl_surface *   Surface                   = nullptr; // Used by Vulkan
  xdg_wm_base *  WindowManangerBase        = nullptr;
  xdg_surface *  DesktopStyleUserInterface = nullptr;
  xdg_toplevel * DesktopWindow             = nullptr;
  // Server side decoration (SSD)
  zxdg_decoration_manager_v1 *  DecorationMananger = nullptr;
  zxdg_toplevel_decoration_v1 * Decoration         = nullptr;
  // Client side decoration (CSD)
  wl_shm *           DecorationSharedMemory = nullptr;
  wl_subcompositor * SubCompositor          = nullptr;
  // Main Bar
  wl_surface *    MainBarSurface    = nullptr;
  wl_subsurface * MainBarSubSurface = nullptr;
  std::uint32_t * MainBarPixels     = nullptr;
  wl_buffer *     MainBarBuffer     = nullptr;
  // Border
  wl_surface *    BorderSurface[8]    = { nullptr, nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr, nullptr };
  wl_subsurface * BorderSubSurface[8] = { nullptr, nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr, nullptr };
  std::uint32_t * BorderPixels[8]     = { nullptr, nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr, nullptr };
  wl_buffer *     BorderBuffer[8]     = { nullptr, nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr, nullptr };
  // Buttons
  wl_surface *    ButtonSurface[3]          = { nullptr, nullptr, nullptr };
  wl_subsurface * ButtonSubSurface[3]       = { nullptr, nullptr, nullptr };
  std::uint32_t * DecorationButtonPixels[3] = { nullptr, nullptr, nullptr };
  wl_buffer *     DecorationButtonBuffer[3] = { nullptr, nullptr, nullptr };
  // Cursor
  wl_surface *      CursorSurface = nullptr;
  wl_cursor_theme * CursorTheme   = nullptr;
};

/*
 * CSD Buttons Layout
 * [MINIMIZE | MAXIMIZE | CLOSE]
 */
#define PATACHE_CSD_MINIMIZE_BUTTON_INDEX 0
#define PATACHE_CSD_MAXIMIZE_BUTTON_INDEX 1
#define PATACHE_CSD_CLOSE_BUTTON_INDEX    2
#define PATACHE_CSD_BUTTON_SIZE           3

/*
 * CSD Borders Layout
 * TOP
 * BOTTOM
 * LEFT
 * RIGHT
 * TOP LEFT
 * TOP RIGHT
 * BOTTOM LEFT
 * BOTTOM RIGHT
 * */
#define PATACHE_CSD_TOP_BORDER_INDEX       0
#define PATACHE_CSD_BOTTOM_BORDER_INDEX    1
#define PATACHE_CSD_BORDER_HORIZONTAL_SIZE 2

#define PATACHE_CSD_LEFT_BORDER_INDEX    2
#define PATACHE_CSD_RIGHT_BORDER_INDEX   3
#define PATACHE_CSD_BORDER_VERTICAL_SIZE 4

#define PATACHE_CSD_TOPLEFT_BORDER_INDEX     4
#define PATACHE_CSD_TOPRIGHT_BORDER_INDEX    5
#define PATACHE_CSD_BOTTOMLEFT_BORDER_INDEX  6
#define PATACHE_CSD_BOTTOMRIGHT_BORDER_INDEX 7
#define PATACHE_CSD_BORDER_SIZE              8
#endif

struct Engine
{
  PATACHE_API Engine (void) = default;

  PATACHE_API Engine (const Patache::EngineCreateInfo &, bool *);
  /*
   * This constructor takes the EngineCreateInfo structure for the initial
   * project configuration. The second argument returns a false boolean if the
   * engine fails to initialize due to a fatal error. If you don't want to use
   * it, pass a nullptr.
   */

  PATACHE_API bool Init (const Patache::EngineCreateInfo &);
  /*
   * This constructor takes the EngineCreateInfo structure for the initial
   * project configuration. It returns a false boolean if the engine fails to
   * initialize due to a fatal error.
   */

  PATACHE_API ~Engine (void);

  Patache::Config configuration{};

  PATACHE_API void HandleEvent (const SDL_Event &); // Patache Events
  PATACHE_API bool BeginRender (SDL_Event &);
  PATACHE_API void EndRender (SDL_Event &);

  // Clear Color
  Patache::ClearColor clearColor{};

  PATACHE_API void ClearColor (const Patache::Color &);
  PATACHE_API void ClearColorRGBA (const float &, const float &, const float &,
                                   const float &);
  PATACHE_API void ClearColorRGBA (const float &, const float &,
                                   const float &);

  // Window
  SDL_Window * GameWindow = nullptr;

#if defined(__linux__)
  Patache::WaylandWindow WaylandWindow{};
#endif

  Patache::VulkanBackend Vulkan{};

#if PATACHE_DEBUG == 1
  Patache::EngineInfo engineInfo{};
#endif
};
}
