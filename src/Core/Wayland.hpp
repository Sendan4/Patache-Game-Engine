// TODO: no recrear los wl_shm_poll todo el tiempo. intentar crearlos con un offset de memoria y
// recrear solo cuando no sea suficiente, asi reduzco el overhead del CSD.
// TODO: usar wl_surface.frame para renderizar. puede ayudar a reducir el overhead y renderizar solo
// cuando sea necesario.
// TODO: mover todas estas variables globales al objeto de ventana.
// TODO: hacer que esta funcion de crear la ventana trabaje con el objeto en si y no globalmente,
// ademas de usar un puntero del motor para conseguir informacion util.

#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <wayland-client.h>
#include <wayland-cursor.h>

// Wayland Protocols
#include <xdg-shell.h>
#include <xdg-decoration-unstable-v1.h>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "Message.hpp"
#include "Vulkan_Swapchain.hpp"

// Window Actions
bool          resizingPending{ false };
extern bool   resize;
extern bool   isFullScreen;
bool          scalePending{ false };
std::uint8_t  scaleInt{ 1 };
std::uint32_t monitorWidth{ 0U };
std::uint32_t monitorHeight{ 0U };

// CSD (Window Client Side Decoration) Actions
static bool         sFocusCSD{ true };
bool                isMaximized{ false };
static std::uint8_t sResizeCSD{ XDG_TOPLEVEL_RESIZE_EDGE_NONE };
static wl_surface * spPointerSelectedSurface{ nullptr };
static SDL_Event    pushWaylandEvent{};

static char borderFileDescriptorName[8U][28U]{
  "Patache-Border-Top",        "Patache-Border-Bottom",      "Patache-Border-Left",
  "Patache-Border-Right",      "Patache-Border-TopLeft",     "Patache-Border-TopRight",
  "Patache-Border-BottomLeft", "Patache-Border-BottomRight",
};
static char shadowFileDescriptorName[8U][28U]{
  "Patache-Shadow-Top",        "Patache-Shadow-Bottom",      "Patache-Shadow-Left",
  "Patache-Shadow-Right",      "Patache-Shadow-TopLeft",     "Patache-Shadow-TopRight",
  "Patache-Shadow-BottomLeft", "Patache-Shadow-BottomRight",
};
static char buttonFileDescriptorName[3U][28U]{
  "Patache-Button-Minimize",
  "Patache-Button-Maximize",
  "Patache-Button-Close",
};
static char mainBarFileDescriptorName[]{ "Patache-MainBar" };

// CSD (Window Client Side Decoration) Pixel Data Buttons
// Minimize
#include "MinimizeButton_LittleEndian_CSD.hpp"
#include "MinimizeButtonHover_LittleEndian_CSD.hpp"
#include "MinimizeButtonPressed_LittleEndian_CSD.hpp"
#include "MinimizeButtonFocusLost_LittleEndian_CSD.hpp"
// Maximize
#include "MaximizeButton_LittleEndian_CSD.hpp"
#include "MaximizeButtonHover_LittleEndian_CSD.hpp"
#include "MaximizeButtonPressed_LittleEndian_CSD.hpp"
#include "MaximizeButtonFocusLost_LittleEndian_CSD.hpp"
#include "MaximizedButton_LittleEndian_CSD.hpp"
#include "MaximizedButtonHover_LittleEndian_CSD.hpp"
#include "MaximizedButtonPressed_LittleEndian_CSD.hpp"
#include "MaximizedButtonFocusLost_LittleEndian_CSD.hpp"
// Close
#include "CloseButton_LittleEndian_CSD.hpp"
#include "CloseButtonHover_LittleEndian_CSD.hpp"
#include "CloseButtonPressed_LittleEndian_CSD.hpp"
#include "CloseButtonFocusLost_LittleEndian_CSD.hpp"
// Shadows
#include "ShadowTop_LittleEndian_CSD.hpp"
#include "ShadowBottom_LittleEndian_CSD.hpp"
#include "ShadowLeft_LittleEndian_CSD.hpp"
#include "ShadowRight_LittleEndian_CSD.hpp"
#include "ShadowTopLeft_LittleEndian_CSD.hpp"
#include "ShadowTopRight_LittleEndian_CSD.hpp"
#include "ShadowBottomLeft_LittleEndian_CSD.hpp"
#include "ShadowBottomRight_LittleEndian_CSD.hpp"

namespace Patache
{
// Sizes
static constexpr std::uint8_t sMainbarHeightCSDSize{ 22U };
static constexpr std::uint8_t sBorderThresholdEdgeCSDSize{ 4U };
static constexpr std::uint8_t sShadowThresholdEdgeCSDSize{ 12U };
// (shadowTresholdEdgeCSDSize ^ 2)
static constexpr std::uint8_t shadowCornerCSDSize{ 144U };
// (shadowTresholdEdgeCSDSize * 4U)
static constexpr std::uint8_t shadowCornerWidthCSDByteSize{ 48U };
// (shadowCornerCSDSize * 4U)
static constexpr std::uint16_t shadowCornerCSDTotalByteSize{ 576U };
// (-borderTresholdEdgeCSDSize - sMainbarHeightCSDSize)
static constexpr std::int8_t windowGeometryPositionY{ -26 };
// (-shadowTresholdEdgeCSDSize - sMainbarHeightCSDSize - borderTresholdEdgeCSDSize)
static constexpr std::int8_t shadowHorizontalPositionY{ -38 };
// (-borderTresholdEdgeCSDSize - shadowTresholdEdgeCSDSize)
static constexpr std::int8_t shadowVerticalPositionX{ -16 };
// (-borderTresholdEdgeCSDSize - sMainbarHeightCSDSize)
static constexpr std::int8_t shadowVerticalPositionY{ -26 };

// Color
static constexpr std::uint32_t mainbarFocusCSDColor{ 0xFFC6A982 };
static constexpr std::uint8_t mainbarFocusCSDColorR{ 0xC6 };
static constexpr std::uint8_t mainbarFocusCSDColorG{ 0xA9 };
static constexpr std::uint8_t mainbarFocusCSDColorB{ 0x82 };
static constexpr std::uint32_t mainbarLostFocusCSDColor{ 0xFF808080 };
static constexpr std::uint32_t borderFocusCSDColor{ 0xFFC6A982 };
static constexpr std::uint8_t borderFocusCSDColorR{ 0xC6 };
static constexpr std::uint8_t borderFocusCSDColorG{ 0xA9 };
static constexpr std::uint8_t borderFocusCSDColorB{ 0x82 };
static constexpr std::uint32_t borderLostFocusCSDColor{ 0xFF808080 };
static constexpr std::uint8_t alphaOpaqueColor{ 0xFF };
static constexpr std::uint8_t alphaTransparentColor{ 0x00 };

// Mouse Buttons
static constexpr std::uint16_t pointerLeftCSDCode{ 272U };
static constexpr std::uint16_t pointerRightCSDCode{ 273U };
static constexpr std::uint16_t pointerWeelButtonCode{ 274U };

// FD random id buffer size
static constexpr std::uint16_t randomizeFdStrBuffSizeExtraNull{ 1034U };
static constexpr std::uint16_t randomizeFdStrBuffSize{ 1034U };
}

#include "WaylandWindow.hpp"

#include <random>

namespace Patache
{
void
RandomizeFileDescriptorName (char * pFileDescriptorName, std::uint32_t size,
                             std::uniform_int_distribution<std::uint32_t> & rFdDist,
                             std::default_random_engine &                   rFdGenerator)
{
  std::uint32_t id{ rFdDist (rFdGenerator) };

  char buff[Patache::randomizeFdStrBuffSizeExtraNull]{};
  std::strncpy (buff, pFileDescriptorName, size);

  std::snprintf (pFileDescriptorName, size, "%s-%d", buff, id);
}

struct SurfaceBufferCleanup
{
  std::size_t     mappedMemSize{ 0U };
  std::uint32_t * pMappedMem{ nullptr };
  std::int32_t    fd{ 0 };
};
}

// wl_buffer_release
static void
BufferRelease (void * pData, wl_buffer * pBuffer)
{
  assert (pData != nullptr && "Patache::SurfaceBufferCleanup is nullptr");

  Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (pData) };

  if (pCleanup->pMappedMem != nullptr)
    munmap (pCleanup->pMappedMem, pCleanup->mappedMemSize);

  if (pCleanup->fd >= 0)
    close (pCleanup->fd);

  wl_buffer_destroy (pBuffer);

  std::free (pCleanup);
}

static constexpr struct wl_buffer_listener wl_buffer_listener{ .release = BufferRelease };

// xdg_wm_base
static void
PingToWindowManangerBase ([[maybe_unused]] void * pData, xdg_wm_base * pWindowManangerBase,
                          std::uint32_t serial)
{
  xdg_wm_base_pong (pWindowManangerBase, serial);
}

static constexpr struct xdg_wm_base_listener sWindowManangerBaseListener{
  .ping = PingToWindowManangerBase
};

/*
 * xdg_surface
 * Here is where the Resize content and buffers work is done for the Wayland
 * Window
 */
static void
DesktopStyleUserInterfaceConfigure (void * pData, xdg_surface * pDesktopStyleUserInterface,
                                    std::uint32_t serial)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  if (resizingPending)
    {
      resize = true;

      SDL_SetWindowSize (pEngine->pGameWindow, pEngine->vulkan.swapchainExtent.width,
                         pEngine->vulkan.swapchainExtent.height);

      // If Server Side Decorations is no available
      if (pEngine->waylandWindow.pDecorationMananger == nullptr
          && pEngine->waylandWindow.pSubCompositor != nullptr)
        {
          // Borders
          if (!isMaximized && !isFullScreen)
            {
              xdg_surface_set_window_geometry (
                  pEngine->waylandWindow.pDesktopStyleUserInterface,
                  -Patache::sBorderThresholdEdgeCSDSize, Patache::windowGeometryPositionY,
                  (pEngine->vulkan.swapchainExtent.width + Patache::sBorderThresholdEdgeCSDSize
                   + Patache::sBorderThresholdEdgeCSDSize),
                  (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize
                   + Patache::sBorderThresholdEdgeCSDSize + Patache::sBorderThresholdEdgeCSDSize));

              // Horizontal Shadow
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTop],
                  -Patache::sBorderThresholdEdgeCSDSize, Patache::shadowHorizontalPositionY);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottom],
                  -Patache::sBorderThresholdEdgeCSDSize,
                  (pEngine->vulkan.swapchainExtent.height + Patache::sBorderThresholdEdgeCSDSize));

              // Vertical Shadow
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eLeft],
                  Patache::shadowVerticalPositionX, Patache::shadowVerticalPositionY);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eRight],
                  (pEngine->vulkan.swapchainExtent.width + Patache::sBorderThresholdEdgeCSDSize),
                  Patache::shadowVerticalPositionY);

              // Corner Shadow
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopLeft],
                  Patache::shadowVerticalPositionX, Patache::shadowHorizontalPositionY);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopRight],
                  (pEngine->vulkan.swapchainExtent.width + Patache::sBorderThresholdEdgeCSDSize),
                  Patache::shadowHorizontalPositionY);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomLeft],
                  Patache::shadowVerticalPositionX,
                  (pEngine->vulkan.swapchainExtent.height + Patache::sBorderThresholdEdgeCSDSize));

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomRight],
                  (pEngine->vulkan.swapchainExtent.width + Patache::sBorderThresholdEdgeCSDSize),
                  (pEngine->vulkan.swapchainExtent.height + Patache::sBorderThresholdEdgeCSDSize));

              // Horizontal
              for (std::uint8_t i{ 0U }; i < Patache::sBorderHorizontalCSDSize; ++i)
                {
                  std::int32_t shadowFileDescriptor{ shm_open (
                      shadowFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (shadowFileDescriptorName[i]);

                  ftruncate (shadowFileDescriptor, (pEngine->vulkan.swapchainExtent.width
                                                    + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                                       * Patache::sShadowThresholdEdgeCSDSize * 4U);

                  std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            (pEngine->vulkan.swapchainExtent.width
                             + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                * Patache::sShadowThresholdEdgeCSDSize * 4U,
                            PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

                  if (i == Patache::BorderIndexCSD::eTop)
                    {
                      for (std::uint32_t i2{ 0U }, i3{ 0U };
                           i2 < (pEngine->vulkan.swapchainExtent.width
                                 + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                    * Patache::sShadowThresholdEdgeCSDSize;
                           ++i2)
                        {
                          if ((i2
                                   % (pEngine->vulkan.swapchainExtent.width
                                      + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                   * Patache::sShadowThresholdEdgeCSDSize
                               == 0)
                              && i3 < Patache::sShadowTopCSDSize)
                            {
                              ++i3;
                            }

                          pBorderPixels[i2] = Patache::sShadowTop[i3];
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2{ 0U }, i3{ 0U };
                           i2 < (pEngine->vulkan.swapchainExtent.width
                                 + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                    * Patache::sShadowThresholdEdgeCSDSize;
                           ++i2)
                        {
                          if ((i2
                                   % (pEngine->vulkan.swapchainExtent.width
                                      + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                   * Patache::sShadowThresholdEdgeCSDSize
                               == 0)
                              && i3 < Patache::sShadowBottomSize)
                            {
                              ++i3;
                            }

                          pBorderPixels[i2] = Patache::sShadowBottom[i3];
                        }
                    }

                  wl_shm_pool * pPoolTest{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
                      (pEngine->vulkan.swapchainExtent.width
                       + (Patache::sBorderThresholdEdgeCSDSize * 2))
                          * Patache::sShadowThresholdEdgeCSDSize * 4U) };

                  wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
                      pPoolTest, 0,
                      (pEngine->vulkan.swapchainExtent.width
                       + (Patache::sBorderThresholdEdgeCSDSize * 2)),
                      Patache::sShadowThresholdEdgeCSDSize,
                      (pEngine->vulkan.swapchainExtent.width
                       + (Patache::sBorderThresholdEdgeCSDSize * 2))
                          * 4U,
                      WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPoolTest);

                  wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                            (pEngine->vulkan.swapchainExtent.width
                                             + (Patache::sBorderThresholdEdgeCSDSize * 2)),
                                            Patache::sShadowThresholdEdgeCSDSize);

                  Patache::SurfaceBufferCleanup * pCleanupShadow{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanupShadow->mappedMemSize = (pEngine->vulkan.swapchainExtent.width
                                                   + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                                  * Patache::sShadowThresholdEdgeCSDSize * 4U;
                  pCleanupShadow->pMappedMem    = pBorderPixels;
                  pCleanupShadow->fd            = shadowFileDescriptor;

                  wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

                  wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
                }

              // Vertical
              for (std::uint8_t i{ Patache::sBorderHorizontalCSDSize };
                   i < Patache::sBorderVerticalCSDSize; ++i)
                {
                  std::int32_t shadowFileDescriptor{ shm_open (
                      shadowFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (shadowFileDescriptorName[i]);

                  ftruncate (shadowFileDescriptor,
                             Patache::sShadowThresholdEdgeCSDSize
                                 * (pEngine->vulkan.swapchainExtent.height
                                    + Patache::sMainbarHeightCSDSize
                                    + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                 * 4U);

                  std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (mmap (
                      nullptr,
                      Patache::sShadowThresholdEdgeCSDSize
                          * (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize
                             + (Patache::sBorderThresholdEdgeCSDSize * 2))
                          * 4U,
                      PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

                  if (i == Patache::BorderIndexCSD::eRight)
                    {
                      for (std::uint32_t i2{ 0U }, i3{ 0U };
                           i2 < (Patache::sShadowThresholdEdgeCSDSize
                                 * (pEngine->vulkan.swapchainExtent.height
                                    + Patache::sMainbarHeightCSDSize
                                    + (Patache::sBorderThresholdEdgeCSDSize * 2)));
                           ++i2, ++i3)
                        {
                          if ((i2 % Patache::sShadowThresholdEdgeCSDSize == 0)
                              || i3 >= Patache::sShadowRightCSDSize)
                            {
                              i3 = 0;
                            }

                          pBorderPixels[i2] = Patache::sShadowRight[i3];
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2{ 0U }, i3{ 0U };
                           i2 < (Patache::sShadowThresholdEdgeCSDSize
                                 * (pEngine->vulkan.swapchainExtent.height
                                    + Patache::sMainbarHeightCSDSize
                                    + (Patache::sBorderThresholdEdgeCSDSize * 2)));
                           ++i2, ++i3)
                        {
                          if ((i2 % Patache::sShadowThresholdEdgeCSDSize == 0)
                              || i3 >= Patache::sShadowRightCSDSize)
                            {
                              i3 = 0;
                            }

                          pBorderPixels[i2] = Patache::sShadowLeftCSD[i3];
                        }
                    }

                  wl_shm_pool * pPoolTest{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
                      Patache::sShadowThresholdEdgeCSDSize
                          * (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize
                             + (Patache::sBorderThresholdEdgeCSDSize * 2))
                          * 4U) };

                  wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
                      pPoolTest, 0, Patache::sShadowThresholdEdgeCSDSize,
                      (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize
                       + (Patache::sBorderThresholdEdgeCSDSize * 2)),
                      Patache::sShadowThresholdEdgeCSDSize * 4U, WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPoolTest);

                  wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                            Patache::sShadowThresholdEdgeCSDSize,
                                            (pEngine->vulkan.swapchainExtent.height
                                             + Patache::sMainbarHeightCSDSize
                                             + (Patache::sBorderThresholdEdgeCSDSize * 2)));

                  Patache::SurfaceBufferCleanup * pCleanupShadow{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanupShadow->mappedMemSize
                      = Patache::sShadowThresholdEdgeCSDSize
                        * (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize
                           + (Patache::sBorderThresholdEdgeCSDSize * 2))
                        * 4U;
                  pCleanupShadow->pMappedMem = pBorderPixels;
                  pCleanupShadow->fd         = shadowFileDescriptor;

                  wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

                  wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
                }

              // Horizontal border
              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTop]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTop], 0,
                      -26);
                }

              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottom]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottom], 0,
                      pEngine->vulkan.swapchainExtent.height);
                }

              // Vertical Border
              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eLeft]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eLeft],
                      -Patache::sBorderThresholdEdgeCSDSize, -Patache::sMainbarHeightCSDSize);
                }

              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eRight]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eRight],
                      pEngine->vulkan.swapchainExtent.width, -Patache::sMainbarHeightCSDSize);
                }

              // Corner Borders
              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopLeft]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopLeft],
                      -Patache::sBorderThresholdEdgeCSDSize, -26);
                }

              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopRight]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopRight],
                      pEngine->vulkan.swapchainExtent.width, -26);
                }

              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomLeft]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow
                          .pBorderSubSurface[Patache::BorderIndexCSD::eBottomLeft],
                      -Patache::sBorderThresholdEdgeCSDSize,
                      pEngine->vulkan.swapchainExtent.height);
                }

              if (pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomRight]
                  != nullptr)
                {
                  wl_subsurface_set_position (
                      pEngine->waylandWindow
                          .pBorderSubSurface[Patache::BorderIndexCSD::eBottomRight],
                      pEngine->vulkan.swapchainExtent.width,
                      pEngine->vulkan.swapchainExtent.height);
                }

              // Horizontal border
              for (std::uint8_t i{ 0U }; i < Patache::sBorderHorizontalCSDSize; ++i)
                {
                  std::int32_t borderFileDescriptor{ shm_open (
                      borderFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (borderFileDescriptorName[i]);

                  ftruncate (borderFileDescriptor, pEngine->vulkan.swapchainExtent.width
                                                       * Patache::sBorderThresholdEdgeCSDSize * 4);

                  // Memory
                  std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            pEngine->vulkan.swapchainExtent.width
                                * Patache::sBorderThresholdEdgeCSDSize * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2{ 0U }; i2 < (pEngine->vulkan.swapchainExtent.width
                                                         * Patache::sBorderThresholdEdgeCSDSize);
                           ++i2)
                        {
                          if (i == Patache::BorderIndexCSD::eBottom)
                            {
                              pBorderPixels[i2] = (Patache::alphaOpaqueColor << 24U)
                                                  | ((Patache::borderFocusCSDColorR
                                                      - (Patache::sMainbarHeightCSDSize * 2U))
                                                     << 16U)
                                                  | ((Patache::borderFocusCSDColorG
                                                      - (Patache::sMainbarHeightCSDSize * 2U))
                                                     << 8U)
                                                  | (Patache::borderFocusCSDColorB
                                                     - (Patache::sMainbarHeightCSDSize * 2U));
                            }
                          else
                            {
                              pBorderPixels[i2] = Patache::borderFocusCSDColor;
                            }
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2{ 0U }; i2 < (pEngine->vulkan.swapchainExtent.width
                                                         * Patache::sBorderThresholdEdgeCSDSize);
                           ++i2)
                        {
                          pBorderPixels[i2] = Patache::borderLostFocusCSDColor;
                        }
                    }

                  wl_shm_pool * pPool{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
                      pEngine->vulkan.swapchainExtent.width * Patache::sBorderThresholdEdgeCSDSize
                          * 4) };

                  wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                      pPool, 0, pEngine->vulkan.swapchainExtent.width,
                      Patache::sBorderThresholdEdgeCSDSize,
                      pEngine->vulkan.swapchainExtent.width * 4, WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPool);

                  // Draw
                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                            pEngine->vulkan.swapchainExtent.width,
                                            Patache::sBorderThresholdEdgeCSDSize);

                  Patache::SurfaceBufferCleanup * pCleanup{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanup->mappedMemSize = pEngine->vulkan.swapchainExtent.width
                                            * Patache::sBorderThresholdEdgeCSDSize * 4;
                  pCleanup->pMappedMem    = pBorderPixels;
                  pCleanup->fd            = borderFileDescriptor;

                  wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

                  wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
                }

              // Vertical Border
              for (std::uint8_t i{ 2U }; i < Patache::sBorderVerticalCSDSize; ++i)
                {
                  std::int32_t borderFileDescriptor{ shm_open (
                      borderFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (borderFileDescriptorName[i]);

                  ftruncate (borderFileDescriptor, Patache::sBorderThresholdEdgeCSDSize
                                                       * (pEngine->vulkan.swapchainExtent.height
                                                          + Patache::sMainbarHeightCSDSize)
                                                       * 4);

                  std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            Patache::sBorderThresholdEdgeCSDSize
                                * (pEngine->vulkan.swapchainExtent.height
                                   + Patache::sMainbarHeightCSDSize)
                                * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2{ 0U }, i3{ 0U };
                           i2 < Patache::sBorderThresholdEdgeCSDSize
                                    * (pEngine->vulkan.swapchainExtent.height
                                       + Patache::sMainbarHeightCSDSize);
                           ++i2)
                        {
                          if (i2 > Patache::sBorderThresholdEdgeCSDSize
                              && i2 % Patache::sBorderThresholdEdgeCSDSize == 0
                              && i2 < (Patache::sBorderThresholdEdgeCSDSize
                                       * Patache::sMainbarHeightCSDSize))
                            {
                              i3 += 2;
                            }

                          pBorderPixels[i2] = (Patache::alphaOpaqueColor << 24U)
                                              | ((Patache::borderFocusCSDColorR - i3) << 16U)
                                              | ((Patache::borderFocusCSDColorG - i3) << 8U)
                                              | (Patache::borderFocusCSDColorB - i3);
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2{ 0U };
                           i2 < Patache::sBorderThresholdEdgeCSDSize
                                    * (pEngine->vulkan.swapchainExtent.height
                                       + Patache::sMainbarHeightCSDSize);
                           ++i2)
                        {
                          pBorderPixels[i2] = Patache::borderLostFocusCSDColor;
                        }
                    }

                  wl_shm_pool * pPool{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
                      Patache::sBorderThresholdEdgeCSDSize
                          * (pEngine->vulkan.swapchainExtent.height
                             + Patache::sMainbarHeightCSDSize)
                          * 4) };

                  wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                      pPool, 0, Patache::sBorderThresholdEdgeCSDSize,
                      (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize),
                      Patache::sBorderThresholdEdgeCSDSize * 4, WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPool);

                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

                  wl_surface_damage_buffer (
                      pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                      Patache::sBorderThresholdEdgeCSDSize,
                      (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize));

                  Patache::SurfaceBufferCleanup * pCleanup{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanup->mappedMemSize
                      = Patache::sBorderThresholdEdgeCSDSize
                        * (pEngine->vulkan.swapchainExtent.height + Patache::sMainbarHeightCSDSize)
                        * 4;
                  pCleanup->pMappedMem = pBorderPixels;
                  pCleanup->fd         = borderFileDescriptor;

                  wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

                  wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
                }

              // Corner Border
              for (std::uint8_t i{ 4U }; i < Patache::sBorderCSDSize; ++i)
                {
                  std::int32_t borderFileDescriptor{ shm_open (
                      borderFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (borderFileDescriptorName[i]);

                  ftruncate (borderFileDescriptor, Patache::sBorderThresholdEdgeCSDSize
                                                       * Patache::sBorderThresholdEdgeCSDSize * 4);

                  std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            Patache::sBorderThresholdEdgeCSDSize
                                * Patache::sBorderThresholdEdgeCSDSize * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2{ 0U }; i2 < (Patache::sBorderThresholdEdgeCSDSize
                                                         * Patache::sBorderThresholdEdgeCSDSize);
                           ++i2)
                        {
                          if (i == Patache::BorderIndexCSD::eBottomLeft
                              || i == Patache::BorderIndexCSD::eBottomRight)
                            {
                              pBorderPixels[i2] = (Patache::alphaOpaqueColor << 24U)
                                                  | ((Patache::borderFocusCSDColorR
                                                      - (Patache::sMainbarHeightCSDSize * 2U))
                                                     << 16U)
                                                  | ((Patache::borderFocusCSDColorG
                                                      - (Patache::sMainbarHeightCSDSize * 2U))
                                                     << 8U)
                                                  | (Patache::borderFocusCSDColorB
                                                     - (Patache::sMainbarHeightCSDSize * 2U));
                            }
                          else
                            {
                              pBorderPixels[i2] = Patache::borderFocusCSDColor;
                            }
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2{ 0U }; i2 < (Patache::sBorderThresholdEdgeCSDSize
                                                         * Patache::sBorderThresholdEdgeCSDSize);
                           ++i2)
                        {
                          pBorderPixels[i2] = Patache::borderLostFocusCSDColor;
                        }
                    }

                  wl_shm_pool * pPool{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
                      Patache::sBorderThresholdEdgeCSDSize * Patache::sBorderThresholdEdgeCSDSize
                          * 4) };

                  wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                      pPool, 0, Patache::sBorderThresholdEdgeCSDSize,
                      Patache::sBorderThresholdEdgeCSDSize,
                      Patache::sBorderThresholdEdgeCSDSize * 4, WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPool);

                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                            Patache::sBorderThresholdEdgeCSDSize,
                                            Patache::sBorderThresholdEdgeCSDSize);

                  Patache::SurfaceBufferCleanup * pCleanup{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanup->mappedMemSize = Patache::sBorderThresholdEdgeCSDSize
                                            * Patache::sBorderThresholdEdgeCSDSize * 4;
                  pCleanup->pMappedMem    = pBorderPixels;
                  pCleanup->fd            = borderFileDescriptor;

                  wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

                  wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
                }
            }

          if (!isFullScreen)
            {
              /*
               * Buttons
               * [MINIMIZE | MAXIMIZE | CLOSE]
               *
               * Recreate the 3 buttons before the main bar for their have the
               * correct position in the main bar
               */

              // Update button position for the new bar size
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMinimize],
                  (pEngine->vulkan.swapchainExtent.width - 87), 2);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMaximize],
                  (pEngine->vulkan.swapchainExtent.width - 57), 2);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eClose],
                  (pEngine->vulkan.swapchainExtent.width - 27), 2);

              for (std::uint8_t i{ 0U }; i < Patache::sButtonCSDSize; ++i)
                {
                  std::int32_t buttonFileDescriptor{ shm_open (
                      buttonFileDescriptorName[i], O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

                  shm_unlink (buttonFileDescriptorName[i]);

                  ftruncate (buttonFileDescriptor, Patache::sCloseButtonCSDSize);

                  // Memory
                  std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                      mmap (nullptr, Patache::sCloseButtonCSDSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, buttonFileDescriptor, 0)) };

                  if (sFocusCSD)
                    {
                      // Focus Colors
                      switch (i)
                        {
                        case Patache::ButtonIndexCSD::eMinimize:
                          for (std::uint32_t i2{ 0U }; i2 < Patache::sMinimizeButtonCSDPixelCount;
                               ++i2)
                            {
                              pButtonPixels[i2] = Patache::sMinimizeButtonCSD[i2];
                            }
                          break;

                        case Patache::ButtonIndexCSD::eMaximize:
                          if (isMaximized)
                            {
                              for (std::uint32_t i2{ 0U };
                                   i2 < Patache::sMaximizeButtonCSDPixelCount; ++i2)
                                {
                                  pButtonPixels[i2] = Patache::sMaximizedButtonCSD[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2{ 0U };
                                   i2 < Patache::sMaximizeButtonCSDPixelCount; ++i2)
                                {
                                  pButtonPixels[i2] = Patache::sMaximizeButtonCSD[i2];
                                }
                            }
                          break;

                        case Patache::ButtonIndexCSD::eClose:
                          for (std::uint32_t i2{ 0U }; i2 < Patache::sCloseButtonCSDPixelCount;
                               ++i2)
                            {
                              pButtonPixels[i2] = Patache::sCloseButtonCSD[i2];
                            }
                          break;
                        }
                    }
                  else
                    {
                      // Lost Focus colors
                      switch (i)
                        {
                        case Patache::ButtonIndexCSD::eMinimize:
                          for (std::uint32_t i2{ 0U }; i2 < Patache::sMinimizeButtonCSDPixelCount;
                               ++i2)
                            {
                              pButtonPixels[i2] = Patache::sMinimizeButtonFocusLostCSD[i2];
                            }
                          break;

                        case Patache::ButtonIndexCSD::eMaximize:
                          if (isMaximized)
                            {
                              for (std::uint32_t i2{ 0U };
                                   i2 < Patache::sMaximizeButtonCSDPixelCount; ++i2)
                                {
                                  pButtonPixels[i2] = Patache::sMaximizedButtonFocusLostCSD[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2{ 0U };
                                   i2 < Patache::sMaximizeButtonCSDPixelCount; ++i2)
                                {
                                  pButtonPixels[i2] = Patache::sMaximizeButtonFocusLostCSD[i2];
                                }
                            }
                          break;

                        case Patache::ButtonIndexCSD::eClose:
                          for (std::uint32_t i2{ 0U }; i2 < Patache::sCloseButtonCSDPixelCount;
                               ++i2)
                            {
                              pButtonPixels[i2] = Patache::sCloseButtonFocusLostCSD[i2];
                            }
                          break;
                        }
                    }

                  wl_shm_pool * pPool{ wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                      Patache::sCloseButtonCSDSize) };

                  wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                      pPool, 0, Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight,
                      Patache::sCloseButtonCSDWidth * 4, WL_SHM_FORMAT_ARGB8888) };

                  wl_shm_pool_destroy (pPool);

                  wl_surface_attach (pEngine->waylandWindow.pButtonSurface[i], pBuffer, 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pButtonSurface[i], 0, 0,
                                            Patache::sCloseButtonCSDWidth,
                                            Patache::sCloseButtonCSDHeight);

                  Patache::SurfaceBufferCleanup * pCleanup{
                    static_cast<Patache::SurfaceBufferCleanup *> (
                        std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
                  };

                  pCleanup->mappedMemSize = Patache::sCloseButtonCSDSize;
                  pCleanup->pMappedMem    = pButtonPixels;
                  pCleanup->fd            = buttonFileDescriptor;

                  wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

                  wl_surface_commit (pEngine->waylandWindow.pButtonSurface[i]);
                }

              /*
               * Main Bar
               * [||||||||||||||||||||||]
               */

              wl_subsurface_set_position (pEngine->waylandWindow.pMainBarSubSurface, 0,
                                          -Patache::sMainbarHeightCSDSize);

              std::int32_t mainBarFileDescriptor{ shm_open (
                  mainBarFileDescriptorName, O_RDWR | O_CREAT | O_EXCL,
                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (mainBarFileDescriptorName);

              ftruncate (mainBarFileDescriptor, pEngine->vulkan.swapchainExtent.width
                                                    * Patache::sMainbarHeightCSDSize * 4);

              std::uint32_t * pMainBarPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr,
                        pEngine->vulkan.swapchainExtent.width * Patache::sMainbarHeightCSDSize * 4,
                        PROT_READ | PROT_WRITE, MAP_SHARED, mainBarFileDescriptor, 0)) };

              if (sFocusCSD)
                {
                  for (std::uint32_t i{ 0U }, i2{ 0U };
                       i < (pEngine->vulkan.swapchainExtent.width * Patache::sMainbarHeightCSDSize);
                       ++i)
                    {
                      if (i > pEngine->vulkan.swapchainExtent.width
                          && i % pEngine->vulkan.swapchainExtent.width == 0)
                        {
                          i2 += 2;
                        }

                      pMainBarPixels[i] = (Patache::alphaOpaqueColor << 24U)
                                          | ((Patache::mainbarFocusCSDColorR - i2) << 16U)
                                          | ((Patache::mainbarFocusCSDColorG - i2) << 8U)
                                          | (Patache::mainbarFocusCSDColorB - i2);
                    }
                }
              else
                {
                  for (std::uint32_t i{ 0U };
                       i < (pEngine->vulkan.swapchainExtent.width * Patache::sMainbarHeightCSDSize);
                       ++i)
                    {
                      pMainBarPixels[i] = Patache::mainbarLostFocusCSDColor;
                    }
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, mainBarFileDescriptor,
                  pEngine->vulkan.swapchainExtent.width * Patache::sMainbarHeightCSDSize * 4) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, pEngine->vulkan.swapchainExtent.width, Patache::sMainbarHeightCSDSize,
                  pEngine->vulkan.swapchainExtent.width * 4, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              wl_surface_attach (pEngine->waylandWindow.pMainBarSurface, pBuffer, 0, 0);
              wl_surface_damage_buffer (pEngine->waylandWindow.pMainBarSurface, 0, 0,
                                        pEngine->vulkan.swapchainExtent.width,
                                        Patache::sMainbarHeightCSDSize);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize
                  = pEngine->vulkan.swapchainExtent.width * Patache::sMainbarHeightCSDSize * 4;
              pCleanup->pMappedMem = pMainBarPixels;
              pCleanup->fd         = mainBarFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }
        }
    }

  xdg_surface_ack_configure (pDesktopStyleUserInterface, serial);
}

static constexpr struct xdg_surface_listener sDesktopStyleUserInterfaceListener{
  .configure = DesktopStyleUserInterfaceConfigure
};

// Output
static void
OutputGeometry ([[maybe_unused]] void * pData, [[maybe_unused]] wl_output * pOutput,
                [[maybe_unused]] int x, [[maybe_unused]] int y, [[maybe_unused]] int physicalWidth,
                [[maybe_unused]] int physicalHeight, [[maybe_unused]] int subPixel,
                [[maybe_unused]] const char * const pMake,
                [[maybe_unused]] const char * const pModel, [[maybe_unused]] int geometry)
{
}

static void
OutputMode ([[maybe_unused]] void * pData, [[maybe_unused]] wl_output * pOutput,
            [[maybe_unused]] std::uint32_t modes, std::int32_t width, std::int32_t height,
            [[maybe_unused]] std::int32_t refresh)
{
  monitorWidth  = width;
  monitorHeight = height;
}

static void
OutputDone (void * pData, [[maybe_unused]] wl_output * pOutput)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  if (scalePending)
    {
      scalePending = false;

      // wl_surface_set_buffer_scale (waylandWindow.pSurface, scale); Dont use this
      if (pEngine->vulkan.device != VK_NULL_HANDLE)
        {
          RecreateSwapchain (pEngine);
        }
    }
}

static void
OutputScale ([[maybe_unused]] void * pData, [[maybe_unused]] wl_output * pOutput,
             std::int32_t factor)
{
  scalePending = true;
  scaleInt     = factor;
}

static void
OutputName ([[maybe_unused]] void * pData, [[maybe_unused]] wl_output * pOutput,
            [[maybe_unused]] const char * const pName)
{
}

static void
OutputDescription ([[maybe_unused]] void * pData, [[maybe_unused]] wl_output * pOutput,
                   [[maybe_unused]] const char * const pDescription)
{
}

static constexpr struct wl_output_listener sOutputListener{ .geometry    = OutputGeometry,
                                                            .mode        = OutputMode,
                                                            .done        = OutputDone,
                                                            .scale       = OutputScale,
                                                            .name        = OutputName,
                                                            .description = OutputDescription };

// Seat
// Using seat input for basic windowing needs
static void
PointerEnter (void * pData, wl_pointer * pPointer, std::uint32_t serial, wl_surface * pSurface,
              [[maybe_unused]] wl_fixed_t surfaceX, [[maybe_unused]] wl_fixed_t surfaceY)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  if (pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      spPointerSelectedSurface = pSurface;
      char cursorType[21]{ 0 };

      // Resize Cursor
      if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTop]
          || pSurface == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTop])
        {
          std::strncpy (cursorType, "top_side", 20ZU);
        }
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottom]
               || pSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eBottom])
        {
          std::strncpy (cursorType, "bottom_side", 20ZU);
        }
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eLeft]
               || pSurface == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eLeft])
        {
          std::strncpy (cursorType, "left_side", 20ZU);
        }
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopLeft]
               || pSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTopLeft])
        {
          std::strncpy (cursorType, "top_left_corner", 20ZU);
        }
      else if (pSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomLeft]
               || pSurface
                      == pEngine->waylandWindow
                             .pShadowSurface[Patache::BorderIndexCSD::eBottomLeft])
        {
          std::strncpy (cursorType, "bottom_left_corner", 20ZU);
        }
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eRight]
               || pSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eRight])
        {
          std::strncpy (cursorType, "right_side", 20ZU);
        }
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopRight]
               || pSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTopRight])
        {
          std::strncpy (cursorType, "top_right_corner", 20ZU);
        }
      else if (pSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomRight]
               || pSurface
                      == pEngine->waylandWindow
                             .pShadowSurface[Patache::BorderIndexCSD::eBottomRight])
        {
          std::strncpy (cursorType, "bottom_right_corner", 20ZU);
        }

      // Normal Cursor
      if (pSurface == pEngine->waylandWindow.pMainBarSurface
          || pSurface == pEngine->waylandWindow.pSurface
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize]
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
        {
          std::strncpy (cursorType, "left_ptr", 19ZU);
        }

      wl_cursor * const pCursor{ wl_cursor_theme_get_cursor (pEngine->waylandWindow.pCursorTheme,
                                                             cursorType) };

      if (pCursor != nullptr)
        {
          wl_cursor_image * const pCursorImage{ pCursor->images[0] };

          wl_pointer_set_cursor (pPointer, serial, pEngine->waylandWindow.pCursorSurface,
                                 pCursorImage->hotspot_x, pCursorImage->hotspot_y);

          wl_surface_attach (pEngine->waylandWindow.pCursorSurface,
                             wl_cursor_image_get_buffer (pCursorImage), 0, 0);

          wl_surface_damage (pEngine->waylandWindow.pCursorSurface, 0, 0, pCursorImage->width,
                             pCursorImage->height);

          wl_surface_commit (pEngine->waylandWindow.pCursorSurface);
        }

      // Button Hover
      if (!isFullScreen)
        {
          /*
           * Buttons
           * [MINIMIZE | MAXIMIZE | CLOSE]
           *
           * Recreate the 3 buttons before the main bar for their have the
           * correct position in the main bar
           */

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose]);

              ftruncate (buttonFileDescriptor, Patache::sCloseButtonCSDSize);

              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sCloseButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        buttonFileDescriptor, 0)) };

              for (std::uint32_t i{ 0U }; i < Patache::sCloseButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sCloseButtonHoverCSD[i];
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sCloseButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight,
                  Patache::sCloseButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], pBuffer,
                  0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], 0, 0,
                  Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sCloseButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize]);

              ftruncate (buttonFileDescriptor, Patache::sMaximizeButtonCSDSize);

              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sMaximizeButtonCSDSize, PROT_READ | PROT_WRITE,
                        MAP_SHARED, buttonFileDescriptor, 0)) };

              if (isMaximized)
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizedButtonHoverCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizeButtonHoverCSD[i];
                    }
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sMaximizeButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight,
                  Patache::sMaximizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              // Draw
              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize],
                  pBuffer, 0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], 0, 0,
                  Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sMaximizeButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize]);

              ftruncate (buttonFileDescriptor, Patache::sMaximizeButtonCSDSize);

              // Memory
              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sMinimizeButtonCSDSize, PROT_READ | PROT_WRITE,
                        MAP_SHARED, buttonFileDescriptor, 0)) };

              for (std::uint32_t i{ 0U }; i < Patache::sMinimizeButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sMinimizeButtonHoverCSD[i];
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sMinimizeButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight,
                  Patache::sMinimizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize],
                  pBuffer, 0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], 0, 0,
                  Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sMinimizeButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }
        }
    }
}

static void
PointerLeave (void * pData, [[maybe_unused]] wl_pointer * pPointer,
              [[maybe_unused]] std::uint32_t serial, wl_surface * pSurface)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  // Restore button colors when the cursor leave the surface of that button
  if (!isFullScreen && pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      /*
       * Buttons
       * [MINIMIZE | MAXIMIZE | CLOSE]
       *
       * Recreate the 3 buttons before the main bar for their have the
       * correct position in the main bar
       */

      // Restore close button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
        {
          std::int32_t buttonFileDescriptor{ shm_open (
              buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose], O_RDWR | O_CREAT | O_EXCL,
              S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose]);

          ftruncate (buttonFileDescriptor, Patache::sCloseButtonCSDSize);

          // Memory
          std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, Patache::sCloseButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buttonFileDescriptor, 0)) };

          if (sFocusCSD)
            {
              for (std::uint32_t i{ 0U }; i < Patache::sCloseButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sCloseButtonCSD[i];
                }
            }
          else
            {
              for (std::uint32_t i{ 0U }; i < Patache::sCloseButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sCloseButtonFocusLostCSD[i];
                }
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                                   buttonFileDescriptor,
                                                   Patache::sCloseButtonCSDSize) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight,
              Patache::sCloseButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose],
                             pBuffer, 0, 0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], 0, 0,
              Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanup->mappedMemSize = Patache::sCloseButtonCSDSize;
          pCleanup->pMappedMem    = pButtonPixels;
          pCleanup->fd            = buttonFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose]);

          wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
        }

      // Restore maximize button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
        {
          std::int32_t buttonFileDescriptor{ shm_open (
              buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize],
              O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize]);

          ftruncate (buttonFileDescriptor, Patache::sMaximizeButtonCSDSize);

          std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, Patache::sMaximizeButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buttonFileDescriptor, 0)) };

          if (sFocusCSD)
            {
              if (isMaximized)
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizedButtonCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizeButtonCSD[i];
                    }
                }
            }
          else
            {
              if (isMaximized)
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizedButtonFocusLostCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizeButtonFocusLostCSD[i];
                    }
                }
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                                   buttonFileDescriptor,
                                                   Patache::sMaximizeButtonCSDSize) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight,
              Patache::sMaximizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], pBuffer, 0,
              0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], 0, 0,
              Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanup->mappedMemSize = Patache::sMaximizeButtonCSDSize;
          pCleanup->pMappedMem    = pButtonPixels;
          pCleanup->fd            = buttonFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]);

          wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
        }

      // Restore minimize button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
        {
          std::int32_t buttonFileDescriptor{ shm_open (
              buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize],
              O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize]);

          ftruncate (buttonFileDescriptor, Patache::sMinimizeButtonCSDSize);

          std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, Patache::sMinimizeButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buttonFileDescriptor, 0)) };

          if (sFocusCSD)
            {
              for (std::uint32_t i{ 0U }; i < Patache::sMinimizeButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sMinimizeButtonCSD[i];
                }
            }
          else
            {
              for (std::uint32_t i{ 0U }; i < Patache::sMinimizeButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sMinimizeButtonFocusLostCSD[i];
                }
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                                   buttonFileDescriptor,
                                                   Patache::sMinimizeButtonCSDSize) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight,
              Patache::sMinimizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], pBuffer, 0,
              0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], 0, 0,
              Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanup->mappedMemSize = Patache::sMinimizeButtonCSDSize;
          pCleanup->pMappedMem    = pButtonPixels;
          pCleanup->fd            = buttonFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize]);

          wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
        }
    }
}

static void
PointerMotion (void * pData, [[maybe_unused]] wl_pointer * pPointer,
               [[maybe_unused]] std::uint32_t time, [[maybe_unused]] wl_fixed_t surfaceX,
               [[maybe_unused]] wl_fixed_t surfaceY)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  // If Server Side Decorations is no available
  if (pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      if (spPointerSelectedSurface
              == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTop]
          || spPointerSelectedSurface
                 == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTop])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottom]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eBottom])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eLeft]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eRight]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eRight])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopLeft]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTopLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopRight]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow.pShadowSurface[Patache::BorderIndexCSD::eTopRight])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomLeft]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow
                             .pShadowSurface[Patache::BorderIndexCSD::eBottomLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        }
      else if (spPointerSelectedSurface
                   == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomRight]
               || spPointerSelectedSurface
                      == pEngine->waylandWindow
                             .pShadowSurface[Patache::BorderIndexCSD::eBottomRight])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        }
      else
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        }
    }
}

static void
PointerPressButton (void * pData, [[maybe_unused]] wl_pointer * pPointer, std::uint32_t serial,
                    [[maybe_unused]] std::uint32_t timePressed, std::uint32_t button,
                    std::uint32_t state)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  // If Server Side Decorations is no available
  // Client window decoration interactions for button left of mouse
  if (button == Patache::pointerLeftCSDCode && !isFullScreen
      && pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      if (state == WL_POINTER_BUTTON_STATE_RELEASED)
        {
          // Minimize Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
            {
              xdg_toplevel_set_minimized (pEngine->waylandWindow.pDesktopWindow);
            }

          // Maximize Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
            {
              if (isMaximized)
                {
                  xdg_toplevel_unset_maximized (pEngine->waylandWindow.pDesktopWindow);

                  for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
                    {
                      pEngine->waylandWindow.pBorderSubSurface[i]
                          = wl_subcompositor_get_subsurface (
                              pEngine->waylandWindow.pSubCompositor,
                              pEngine->waylandWindow.pBorderSurface[i],
                              pEngine->waylandWindow.pSurface);

                      pEngine->waylandWindow.pShadowSubSurface[i]
                          = wl_subcompositor_get_subsurface (
                              pEngine->waylandWindow.pSubCompositor,
                              pEngine->waylandWindow.pShadowSurface[i],
                              pEngine->waylandWindow.pSurface);
                    }
                }
              else
                {
                  for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
                    {
                      wl_subsurface_destroy (pEngine->waylandWindow.pBorderSubSurface[i]);
                      wl_subsurface_destroy (pEngine->waylandWindow.pShadowSubSurface[i]);
                    }

                  xdg_toplevel_set_maximized (pEngine->waylandWindow.pDesktopWindow);
                }

              isMaximized ^= true;
            }

          // Close Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
            {
              pushWaylandEvent.type = SDL_EVENT_QUIT;
              SDL_PushEvent (&pushWaylandEvent);
            }
        }

      if (state == WL_POINTER_BUTTON_STATE_PRESSED)
        {
          if (spPointerSelectedSurface == pEngine->waylandWindow.pMainBarSurface && !isMaximized)
            {
              xdg_toplevel_move (pEngine->waylandWindow.pDesktopWindow,
                                 pEngine->waylandWindow.pInput, serial);
            }
          else if (sResizeCSD != XDG_TOPLEVEL_RESIZE_EDGE_NONE)
            {
              xdg_toplevel_resize (pEngine->waylandWindow.pDesktopWindow,
                                   pEngine->waylandWindow.pInput, serial, sResizeCSD);
            }

          // Minimize Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMinimize]);

              ftruncate (buttonFileDescriptor, Patache::sMinimizeButtonCSDSize);

              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sMinimizeButtonCSDSize, PROT_READ | PROT_WRITE,
                        MAP_SHARED, buttonFileDescriptor, 0)) };

              for (std::uint32_t i{ 0U }; i < Patache::sMinimizeButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sMinimizeButtonPressedCSD[i];
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sMinimizeButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight,
                  Patache::sMinimizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize],
                  pBuffer, 0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], 0, 0,
                  Patache::sMinimizeButtonCSDWidth, Patache::sMinimizeButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sMinimizeButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          // Maximize Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eMaximize]);

              ftruncate (buttonFileDescriptor, Patache::sMaximizeButtonCSDSize);

              // Memory
              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sMaximizeButtonCSDSize, PROT_READ | PROT_WRITE,
                        MAP_SHARED, buttonFileDescriptor, 0)) };

              if (isMaximized)
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizedButtonPressedCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i{ 0U }; i < Patache::sMaximizeButtonCSDPixelCount; ++i)
                    {
                      pButtonPixels[i] = Patache::sMaximizeButtonPressedCSD[i];
                    }
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sMaximizeButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight,
                  Patache::sMaximizeButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              // Draw
              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize],
                  pBuffer, 0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], 0, 0,
                  Patache::sMaximizeButtonCSDWidth, Patache::sMaximizeButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sMaximizeButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          // Close Button
          if (spPointerSelectedSurface
              == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
            {
              std::int32_t buttonFileDescriptor{ shm_open (
                  buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose],
                  O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

              shm_unlink (buttonFileDescriptorName[Patache::ButtonIndexCSD::eClose]);

              ftruncate (buttonFileDescriptor, Patache::sCloseButtonCSDSize);

              std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
                  mmap (nullptr, Patache::sCloseButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        buttonFileDescriptor, 0)) };

              for (std::uint32_t i{ 0U }; i < Patache::sCloseButtonCSDPixelCount; ++i)
                {
                  pButtonPixels[i] = Patache::sCloseButtonPressedCSD[i];
                }

              wl_shm_pool * pPool{ wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, buttonFileDescriptor,
                  Patache::sCloseButtonCSDSize) };

              wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
                  pPool, 0, Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight,
                  Patache::sCloseButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

              wl_shm_pool_destroy (pPool);

              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], pBuffer,
                  0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], 0, 0,
                  Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight);

              Patache::SurfaceBufferCleanup * pCleanup{
                static_cast<Patache::SurfaceBufferCleanup *> (
                    std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
              };

              pCleanup->mappedMemSize = Patache::sCloseButtonCSDSize;
              pCleanup->pMappedMem    = pButtonPixels;
              pCleanup->fd            = buttonFileDescriptor;

              wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }
        }
    }
}

// This function is not used, but is here to silence GNU Compiler
static void
PointerAxis ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
             [[maybe_unused]] std::uint32_t time, [[maybe_unused]] std::uint32_t axis,
             [[maybe_unused]] wl_fixed_t value)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
Frame ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisSource ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
            [[maybe_unused]] std::uint32_t axisSource)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisStop ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
          [[maybe_unused]] std::uint32_t time, [[maybe_unused]] std::uint32_t axis)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisDiscrete ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
              [[maybe_unused]] std::uint32_t axis, [[maybe_unused]] std::int32_t discrete)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisValue120 ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
              [[maybe_unused]] std::uint32_t axis, [[maybe_unused]] std::int32_t axis120)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
AxisRelativeDirection ([[maybe_unused]] void * pData, [[maybe_unused]] wl_pointer * pPointer,
                       [[maybe_unused]] std::uint32_t axis,
                       [[maybe_unused]] std::uint32_t relativeDirection)
{
}

static constexpr struct wl_pointer_listener sPointerListener{
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
GetInputCapabilities (void * pData, wl_seat * pInput, std::uint32_t capabilities)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  // If Server Side Decorations is no available
  // Using pointer input for client side window decorations (CSD)
  if (capabilities & WL_SEAT_CAPABILITY_POINTER
      && pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      wl_pointer_add_listener (wl_seat_get_pointer (pInput), &sPointerListener, pData);

      pEngine->waylandWindow.pCursorSurface
          = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);
    }
}

// This function is not used, but is here to silence GNU Compiler
static void
Name ([[maybe_unused]] void * pData, [[maybe_unused]] wl_seat * pInput,
      [[maybe_unused]] const char * pName)
{
}

static constexpr struct wl_seat_listener sInputListener{ .capabilities = GetInputCapabilities,
                                                         .name         = Name };

// Registry
// Get Wayland handles from protocols and core
static void
AddObject (void * pData, wl_registry * pRegistry, std::uint32_t name, const char * pInterface,
           [[maybe_unused]] std::uint32_t version)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  if (std::strcmp (pInterface, wl_compositor_interface.name) == 0)
    {
      pEngine->waylandWindow.pCompositor = static_cast<wl_compositor *> (
          wl_registry_bind (pRegistry, name, &wl_compositor_interface, 4U));
    }
  else if (std::strcmp (pInterface, xdg_wm_base_interface.name) == 0)
    {
      pEngine->waylandWindow.pWindowManangerBase = static_cast<xdg_wm_base *> (
          wl_registry_bind (pRegistry, name, &xdg_wm_base_interface, 3U));

      xdg_wm_base_add_listener (pEngine->waylandWindow.pWindowManangerBase,
                                &sWindowManangerBaseListener, nullptr);
    }
  else if (std::strcmp (pInterface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
      // Server Side Decoration (SSD)
      pEngine->waylandWindow.pDecorationMananger = static_cast<zxdg_decoration_manager_v1 *> (
          wl_registry_bind (pRegistry, name, &zxdg_decoration_manager_v1_interface, 1U));
    }
  else if (std::strcmp (pInterface, wl_subcompositor_interface.name) == 0)
    {
      // Client Side Decoration (CSD)
      pEngine->waylandWindow.pSubCompositor = static_cast<wl_subcompositor *> (
          wl_registry_bind (pRegistry, name, &wl_subcompositor_interface, 1U));
    }
  else if (std::strcmp (pInterface, wl_shm_interface.name) == 0)
    {
      pEngine->waylandWindow.pDecorationSharedMemory
          = static_cast<wl_shm *> (wl_registry_bind (pRegistry, name, &wl_shm_interface, 1U));

      pEngine->waylandWindow.pCursorTheme
          = wl_cursor_theme_load (nullptr, 24U, pEngine->waylandWindow.pDecorationSharedMemory);
    }
  else if (std::strcmp (pInterface, wl_seat_interface.name) == 0)
    {
      pEngine->waylandWindow.pInput
          = static_cast<wl_seat *> (wl_registry_bind (pRegistry, name, &wl_seat_interface, 5U));

      wl_seat_add_listener (pEngine->waylandWindow.pInput, &sInputListener, pData);
    }
  else if (std::strcmp (pInterface, wl_output_interface.name) == 0)
    {
      pEngine->waylandWindow.pOutput
          = static_cast<wl_output *> (wl_registry_bind (pRegistry, name, &wl_output_interface, 2U));

      wl_output_add_listener (pEngine->waylandWindow.pOutput, &sOutputListener, pEngine);
    }
}

// This function is not used, but is here to silence GNU Compiler
static void
RemoveObject ([[maybe_unused]] void * pData, [[maybe_unused]] wl_registry * pRegistry,
              [[maybe_unused]] std::uint32_t name)
{
}

static constexpr struct wl_registry_listener sRegistryListener{ .global        = AddObject,
                                                                .global_remove = RemoveObject };

bool          returnFromFullscreen{ false };
bool          waitForFullscreen{ false };
std::uint32_t widthWindoned{ 720U };
std::uint32_t heightWindoned{ 480U };

// xdg_toplevel
static void
GetWindowSize (void * pData, [[maybe_unused]] xdg_toplevel * pDesktopWindow, std::int32_t width,
               std::int32_t height, wl_array * pStates)
{
  Patache::Engine * pEngine{ static_cast<Patache::Engine *> (pData) };

  if (pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      sFocusCSD = false;

      // wl_array_for_each no funciona en C++ debido a hacer conversiones raras de void * que
      // igual estoy haciendo aca, la cuestion es que no configure el compilador como permisivo
      // (-fpermissive). asi que toco hacer mi propio wl_array_for_each()
      enum xdg_toplevel_state * pState;

      for (std::uint32_t pos{ 0U }; pStates->size > 0 && pos < pStates->size;
           pos += sizeof (xdg_toplevel_state))
        {
          pState = static_cast<xdg_toplevel_state *> (
              (static_cast<xdg_toplevel_state *> (pStates->data) + pos));

          if (*pState == 0)
            break;

          if (*pState == XDG_TOPLEVEL_STATE_ACTIVATED || *pState == XDG_TOPLEVEL_STATE_RESIZING)
            {
              sFocusCSD       = true;
              resizingPending = true;

              if (isMaximized)
                {
                  xdg_toplevel_unset_maximized (pEngine->waylandWindow.pDesktopWindow);

                  // Border Window
                  for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
                    {
                      pEngine->waylandWindow.pBorderSubSurface[i]
                          = wl_subcompositor_get_subsurface (
                              pEngine->waylandWindow.pSubCompositor,
                              pEngine->waylandWindow.pBorderSurface[i],
                              pEngine->waylandWindow.pSurface);

                      pEngine->waylandWindow.pShadowSubSurface[i]
                          = wl_subcompositor_get_subsurface (
                              pEngine->waylandWindow.pSubCompositor,
                              pEngine->waylandWindow.pShadowSurface[i],
                              pEngine->waylandWindow.pSurface);
                    }

                  isMaximized = false;
                }
            }

          if (*pState == XDG_TOPLEVEL_STATE_MAXIMIZED)
            {
              sFocusCSD       = true;
              resizingPending = true;

              if (!isMaximized)
                {
                  xdg_toplevel_set_maximized (pEngine->waylandWindow.pDesktopWindow);

                  // Border Window
                  for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
                    {
                      if (pEngine->waylandWindow.pBorderSubSurface[i] != nullptr)
                        {
                          wl_subsurface_destroy (pEngine->waylandWindow.pBorderSubSurface[i]);
                          wl_subsurface_destroy (pEngine->waylandWindow.pShadowSubSurface[i]);
                        }
                    }

                  isMaximized = true;
                }
            }

          // Esto es por weston, no podia simplemente decirle que desmaximize la ventana y que la
          // ponga en pantalla completa de una, tengo que primero desmaximizar y luego solicitarle
          // al compositor que coloque la ventana en pantalla completa, no se pueden hacer a la vez
          // con el exigente de Weston, hay que solicitarlo uno por uno y esperar a que los estados
          // llegen.
          if (waitForFullscreen)
            {
              if (*pState == XDG_TOPLEVEL_STATE_FULLSCREEN)
                {
                  waitForFullscreen = false;
                }
              else
                {
                  // From window
                  for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
                    {
                      wl_subsurface_destroy (pEngine->waylandWindow.pShadowSubSurface[i]);
                      wl_subsurface_destroy (pEngine->waylandWindow.pBorderSubSurface[i]);
                    }

                  wl_subsurface_destroy (pEngine->waylandWindow.pMainBarSubSurface);

                  for (std::uint8_t i{ 0U }; i < 3U; ++i)
                    {
                      wl_subsurface_destroy (pEngine->waylandWindow.pButtonSubSurface[i]);
                    }

                  isFullScreen = true;
                  xdg_toplevel_set_fullscreen (pEngine->waylandWindow.pDesktopWindow, nullptr);
                }
            }
        }

      if (returnFromFullscreen)
        {
          // si el compositor no devuelve un nuevo tamaño, toca inprovisar
          if (!isFullScreen && !isMaximized)
            {
              if (widthWindoned >= monitorWidth || heightWindoned >= monitorHeight)
                {
                  widthWindoned  = monitorWidth;
                  heightWindoned = monitorHeight;
                }

              width  = widthWindoned;
              height = heightWindoned;

              pEngine->vulkan.swapchainExtent.width = width - Patache::sBorderThresholdEdgeCSDSize
                                                      - Patache::sBorderThresholdEdgeCSDSize;
              pEngine->vulkan.swapchainExtent.height
                  = height - Patache::sMainbarHeightCSDSize
                    - (Patache::sBorderThresholdEdgeCSDSize * 2U);
            }

          resizingPending      = true;
          returnFromFullscreen = false;
        }
    }

  // Resize vulkan swapchain
  if (width != 0 && height != 0)
    {
      resizingPending = true;

      if (pEngine->waylandWindow.pDecorationMananger == nullptr
          && pEngine->waylandWindow.pSubCompositor != nullptr)
        {
          // Client Side Decoration (CSD)
          if (!isFullScreen && !isMaximized)
            {
              // CSD Window
              width  = ((width + scaleInt - 1) / scaleInt) * scaleInt;
              height = ((height + scaleInt - 1) / scaleInt) * scaleInt;

              pEngine->vulkan.swapchainExtent.width = width - Patache::sBorderThresholdEdgeCSDSize
                                                      - Patache::sBorderThresholdEdgeCSDSize;

              pEngine->vulkan.swapchainExtent.height
                  = height - Patache::sMainbarHeightCSDSize
                    - (Patache::sBorderThresholdEdgeCSDSize * 2U);

              pEngine->vulkan.swapchainExtent.width
                  = ((pEngine->vulkan.swapchainExtent.width + scaleInt - 1) / scaleInt) * scaleInt;

              pEngine->vulkan.swapchainExtent.height
                  = ((pEngine->vulkan.swapchainExtent.height + scaleInt - 1) / scaleInt) * scaleInt;

              widthWindoned  = width;
              heightWindoned = height;
            }
          else if (isMaximized && !isFullScreen)
            {
              // CSD Maximized
              pEngine->vulkan.swapchainExtent.width  = width;
              pEngine->vulkan.swapchainExtent.height = height - Patache::sMainbarHeightCSDSize;

              xdg_surface_set_window_geometry (pEngine->waylandWindow.pDesktopStyleUserInterface, 0,
                                               -Patache::sMainbarHeightCSDSize, width, height);
            }
          else
            {
              // CSD FullScreen
              pEngine->vulkan.swapchainExtent.width  = width;
              pEngine->vulkan.swapchainExtent.height = height;

              xdg_surface_set_window_geometry (pEngine->waylandWindow.pDesktopStyleUserInterface, 0,
                                               0, width, height);
            }
        }
      else
        {
          // Server Side Decoration (SSD)
          pEngine->vulkan.swapchainExtent.width  = width;
          pEngine->vulkan.swapchainExtent.height = height;
        }
    }
  else
    {
      // si el compositor no devuelve un nuevo tamaño, toca inprovisar
      if (!isFullScreen && !isMaximized)
        {
          // CSD Window
          width  = widthWindoned;
          height = heightWindoned;

          pEngine->vulkan.swapchainExtent.width
              = width - Patache::sBorderThresholdEdgeCSDSize - Patache::sBorderThresholdEdgeCSDSize;

          pEngine->vulkan.swapchainExtent.height = height - Patache::sMainbarHeightCSDSize
                                                   - (Patache::sBorderThresholdEdgeCSDSize * 2U);
        }
    }
}

static void
CloseWindow ([[maybe_unused]] void * pData, [[maybe_unused]] xdg_toplevel * pDesktopWindow)
{
  pushWaylandEvent.type = SDL_EVENT_QUIT;
  SDL_PushEvent (&pushWaylandEvent);
}

// This function is not used, but is here to silence GNU Compiler
static void
ConfigureBounds ([[maybe_unused]] void * pData, [[maybe_unused]] xdg_toplevel * pDesktopWindow,
                 [[maybe_unused]] std::int32_t width, [[maybe_unused]] std::int32_t height)
{
}

// This function is not used, but is here to silence GNU Compiler
static void
WindowManangerCapabilities ([[maybe_unused]] void *         pData,
                            [[maybe_unused]] xdg_toplevel * pDesktopWindow,
                            [[maybe_unused]] wl_array *     pStates)
{
}

static constexpr struct xdg_toplevel_listener sDesktopWindowListener{
  .configure        = GetWindowSize,
  .close            = CloseWindow,
  .configure_bounds = ConfigureBounds,
  .wm_capabilities  = WindowManangerCapabilities
};
