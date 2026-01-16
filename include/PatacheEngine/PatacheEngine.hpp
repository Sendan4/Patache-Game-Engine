#pragma once

#include <cstdint>

// cglm warnings
#if defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #include <cglm/cglm.h>
  #include <cglm/call.h>
  #pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable : 4100)
  #include <cglm/cglm.h>
  #include <cglm/call.h>
  #pragma warning(pop)
#endif

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
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
#if PATACHE_DEBUG == 1
  #include "PatacheEngine/StructDebugInfo.hpp"
#endif

namespace Patache
{
enum class ProjectGraphics : bool
{
  Mode2D, // 2D
  Mode3D  // 3D
};

// you can use nullptr to not use the options or not assign them at all.
struct EngineCreateInfo
{
  const char * const  pGameName   = nullptr; // Name of the game // It is optional.
  const std::uint32_t gameVersion = 0;
  // Vulkan version style: major.minor.patch.variant // It is
  // optional.
  const char * const pWindowTitle = nullptr;
  // Initial title of the window. // It is optional.
  const Patache::ProjectGraphics projectGraphicsMode = Patache::ProjectGraphics::Mode2D;
  // Determines whether the project uses 2D or 3D graphics.
  // It is mandatory.
  const char * const pWindowIconPath = nullptr;
  // Initial icon of the window. // It is optional.
  // Use bitmap (.bmp) format/codec. // It is optional.
  const std::uint32_t memRenderSizePerImage = 262144U;
  const std::uint32_t buffStagingSize       = 262144U;
};

// Float32 format
struct ClearColor
{
  float r = 0.0F; // Red
  float g = 0.0F; // Green
  float b = 0.0F; // Blue
  float a = 1.0F; // Alpha
};

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
struct WaylandWindow
{
  // Globals
  wl_display *    pDisplay{ nullptr };
  wl_compositor * pCompositor{ nullptr };
  wl_seat *       pInput{ nullptr };
  // Main Surface for graphics
  wl_surface *   pSurface{ nullptr }; // Used by Vulkan
  xdg_wm_base *  pWindowManangerBase{ nullptr };
  xdg_surface *  pDesktopStyleUserInterface{ nullptr };
  xdg_toplevel * pDesktopWindow{ nullptr };
  // Server side decoration (SSD)
  zxdg_decoration_manager_v1 *  pDecorationMananger{ nullptr };
  zxdg_toplevel_decoration_v1 * pDecoration{ nullptr };
  // Client side decoration (CSD)
  wl_shm *           pDecorationSharedMemory{ nullptr };
  wl_subcompositor * pSubCompositor{ nullptr };
  // Main Bar
  wl_surface *    pMainBarSurface{ nullptr };
  wl_subsurface * pMainBarSubSurface{ nullptr };
  // Border
  wl_surface *    pBorderSurface[8U]{ nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr, nullptr, nullptr };
  wl_subsurface * pBorderSubSurface[8U]{ nullptr, nullptr, nullptr, nullptr,
                                         nullptr, nullptr, nullptr, nullptr };
  // Buttons
  wl_surface *    pButtonSurface[3U]{ nullptr, nullptr, nullptr };
  wl_subsurface * pButtonSubSurface[3U]{ nullptr, nullptr, nullptr };
  // Cursor
  wl_surface *      pCursorSurface{ nullptr };
  wl_cursor_theme * pCursorTheme{ nullptr };
};

  /*
   * CSD Buttons Layout
   * [MINIMIZE | MAXIMIZE | CLOSE]
   */
  #define PATACHE_BUTTON_CSD_SIZE 3U

enum ButtonIndexCSD : std::uint8_t
{
  eMinimize,
  eMaximize,
  eClose
};

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
   */
  #define PATACHE_BORDER_HORIZONTAL_CSD_SIZE 2U
  #define PATACHE_BORDER_VERTICAL_CSD_SIZE   4U
  #define PATACHE_BORDER_CSD_SIZE            8U

enum BorderIndexCSD : std::uint8_t
{
  eTop,
  eBottom,
  eLeft,
  eRight,
  eTopLeft,
  eTopRight,
  eBottomLeft,
  eBottomRight
};
#endif

struct Vertex2D
{
  vec2 pos   = { 0.0F, 0.0F };
  vec3 color = { 1.0F, 1.0F, 1.0F };
  // Default color white
};

// Triangle
struct Triangle
{
  Vertex2D vertex[3] = { { { 0.0F, -0.5F }, { 1.0F, 1.0F, 1.0F } },
                         { { 0.5F, 0.5F }, { 1.0F, 1.0F, 1.0F } },
                         { { -0.5F, 0.5F }, { 1.0F, 1.0F, 1.0F } } };

  vk::DeviceSize   bufferOffset{ 0U };
  static constexpr std::uint_fast64_t size{ sizeof (Patache::Vertex2D) * 3 };

  PATACHE_API constexpr void
  SetColorRGB (const std::uint8_t & rSide, const float & rRed, const float & rBlue,
               const float & rGreen)
  {
    assert ("A triangle only have 3 sides (0 - 2)" && (rSide <= 2U));

    vertex[rSide].color[0] = rRed;
    vertex[rSide].color[1] = rBlue;
    vertex[rSide].color[2] = rGreen;
  }
};

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

  // Window
  SDL_Window * pGameWindow = nullptr;

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
  Patache::WaylandWindow waylandWindow{};
#endif

  Patache::VulkanBackend vulkan{};

#if PATACHE_DEBUG == 1
  Patache::EngineInfo debugInfo{};
#endif

  Patache::Config configuration{};

  PATACHE_API void HandleEvent (const SDL_Event &); // Patache Events
  PATACHE_API bool BeginRender (SDL_Event &);
  PATACHE_API void EndRender (SDL_Event &);

  // Clear Color
  Patache::ClearColor clearColor{};

  PATACHE_API void ClearColor (const Patache::Color &);
  PATACHE_API void ClearColorRGBA (const float &, const float &, const float &, const float &);
  PATACHE_API void ClearColorRGBA (const float &, const float &, const float &);

  PATACHE_API bool Bsalloc (Patache::Triangle * const);
  PATACHE_API void Bsfree (Patache::Triangle * const);

  PATACHE_API void Draw (Patache::Triangle * const);
};

// Versioning management in the style of Vulkan. // major.minor.patch.variant
constexpr std::uint32_t
          MakeVersion (const std::uint8_t & rMajor, const std::uint16_t & rMinor,
                       const std::uint16_t & rPatch, const std::uint8_t & rVariant)
{
  return ((static_cast<std::uint32_t> (rVariant)) << 29U
          | (static_cast<std::uint32_t> (rMajor)) << 22U
          | (static_cast<std::uint32_t> (rMinor)) << 12U | static_cast<std::uint32_t> (rPatch));
}

constexpr std::uint8_t
          GetVersionMajor (const std::uint32_t & rMajor)
{
  // 6 bits
  return rMajor >> 22U & 0b1111111;
}

constexpr std::uint16_t
          GetVersionMinor (const std::uint32_t & rMinor)
{
  // 9 bits
  return rMinor >> 12U & 0b1111111111;
}

constexpr std::uint16_t
          GetVersionPatch (const std::uint32_t & rPatch)
{
  // 11 bits
  return rPatch & 0b111111111111;
}

constexpr std::uint8_t
          GetVersionVariant (const std::uint32_t & rVariant)
{
  // 5 bits
  return rVariant >> 29U & 0b111111;
}
}
