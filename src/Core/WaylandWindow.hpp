#include <future>
#include <functional>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <wayland-client.h>
#include <wayland-cursor.h>

// Wayland Protocols
#include <xdg-shell.h>
#include <xdg-decoration-unstable-v1.h>

#include <SDL3/SDL.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "Message.hpp"

// Window Actions
bool        resizingPending = false;
extern bool resize;
extern bool isFullScreen;

// CSD (Window Client Side Decoration) Actions
static bool         sFocusCSD                = true;
bool                isMaximized              = false;
static std::uint8_t sResizeCSD               = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
static wl_surface * spPointerSelectedSurface = nullptr;
static SDL_Event    pushWaylandEvent{};

// CSD (Window Client Side Decoration) Pixel pData Buttons
// Minimize
#include "MinimizeButton_LittleEndian_CSD.hpp"
#include "MinimizeButtonHover_LittleEndian_CSD.hpp"
#include "MinimizeButtonFocusLost_LittleEndian_CSD.hpp"
// Maximize
#include "MaximizeButton_LittleEndian_CSD.hpp"
#include "MaximizeButtonHover_LittleEndian_CSD.hpp"
#include "MaximizedButton_LittleEndian_CSD.hpp"
#include "MaximizedButtonHover_LittleEndian_CSD.hpp"
#include "MaximizeButtonFocusLost_LittleEndian_CSD.hpp"
#include "MaximizedButtonFocusLost_LittleEndian_CSD.hpp"
// Close
#include "CloseButton_LittleEndian_CSD.hpp"
#include "CloseButtonHover_LittleEndian_CSD.hpp"
#include "CloseButtonFocusLost_LittleEndian_CSD.hpp"

// Sizes
#define PATACHE_MAINBAR_HEIGHT_CSD_SIZE       22
#define PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE 4

// Color
#define PATACHE_MAINBAR_FOCUS_CSD_COLOR     0xFFC6A982
#define PATACHE_MAINBAR_LOSTFOCUS_CSD_COLOR 0xFF808080
#define PATACHE_BORDER_CSD_COLOR            0xFFF1E9E0
#define PATACHE_BORDER_LOSTFOCUS_CSD_COLOR  0xFFF2F2F2

// Mouse Buttons
#define PATACHE_POINTER_LEFT_CSD_CODE       272
#define PATACHE_POINTER_RIGHT_CSD_CODE      273
#define PATACHE_POINTER_WEELBUTTON_CSD_CODE 274

#include "WaylandWindow_Funcs.hpp"

// xdg_wm_base
static void
PingToWindowManangerBase (void * pData, xdg_wm_base * pWindowManangerBase, std::uint32_t serial)
{
  static_cast<void> (pData);
  xdg_wm_base_pong (pWindowManangerBase, serial);
}

static constexpr struct xdg_wm_base_listener sWindowManangerBaseListener
    = { .ping = PingToWindowManangerBase };

/*
 * xdg_surface
 * Here is where the Resize content and buffers work is done for the Wayland
 * Window
 */
static void
DesktopStyleUserInterfaceConfigure (void * pData, xdg_surface * pDesktopStyleUserInterface,
                                    std::uint32_t serial)
{
  xdg_surface_ack_configure (pDesktopStyleUserInterface, serial);
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

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
              // Horizontal border
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTop], 0, -26);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottom], 0,
                  pEngine->vulkan.swapchainExtent.height);

              // Vertical Border
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eLeft],
                  -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eRight],
                  pEngine->vulkan.swapchainExtent.width, -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

              // Corner Borders
              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopLeft],
                  -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, -26);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopRight],
                  pEngine->vulkan.swapchainExtent.width, -26);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomLeft],
                  -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, pEngine->vulkan.swapchainExtent.height);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomRight],
                  pEngine->vulkan.swapchainExtent.width, pEngine->vulkan.swapchainExtent.height);

              // Horizontal border
              for (std::uint8_t i = 0; i < PATACHE_BORDER_HORIZONTAL_CSD_SIZE; ++i)
                {
                  // File descriptor
                  char borderFileNameCSD[] = "Patache-Border";

                  std::int32_t borderFileDescriptorCSD
                      = shm_open (borderFileNameCSD, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

                  shm_unlink (borderFileNameCSD);

                  ftruncate (borderFileDescriptorCSD, pEngine->vulkan.swapchainExtent.width
                                                          * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * 4);

                  // Memory
                  pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            pEngine->vulkan.swapchainExtent.width
                                * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptorCSD, 0));

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2 = 0; i2 < (pEngine->vulkan.swapchainExtent.width
                                                       * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
                           ++i2)
                        {
                          pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0; i2 < (pEngine->vulkan.swapchainExtent.width
                                                       * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
                           ++i2)
                        pEngine->waylandWindow.pBorderPixels[i][i2]
                            = PATACHE_BORDER_LOSTFOCUS_CSD_COLOR;
                    }

                  wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptorCSD,
                      pEngine->vulkan.swapchainExtent.width * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                          * 4);

                  pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
                      pDecorationBorderPool, 0, pEngine->vulkan.swapchainExtent.width,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                      pEngine->vulkan.swapchainExtent.width * 4, WL_SHM_FORMAT_ARGB8888);

                  // Draw
                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                                     pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                            pEngine->vulkan.swapchainExtent.width,
                                            PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

                  wl_shm_pool_destroy (pDecorationBorderPool);
                  close (borderFileDescriptorCSD);

                  wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
                }

              // Vertical Border
              for (std::uint8_t i = 2; i < PATACHE_BORDER_VERTICAL_CSD_SIZE; ++i)
                {
                  // File descriptor
                  char borderFileNameCSD[] = "Patache-Border";

                  std::int32_t borderFileDescriptorCSD
                      = shm_open (borderFileNameCSD, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

                  shm_unlink (borderFileNameCSD);

                  ftruncate (borderFileDescriptorCSD, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * (pEngine->vulkan.swapchainExtent.height
                                                             + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                                                          * 4);

                  // Memory
                  pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                * (pEngine->vulkan.swapchainExtent.height
                                   + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                                * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptorCSD, 0));

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2 = 0; i2 < PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * (pEngine->vulkan.swapchainExtent.height
                                                             + PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
                           ++i2)
                        {
                          pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0; i2 < PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * (pEngine->vulkan.swapchainExtent.height
                                                             + PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
                           ++i2)
                        {
                          pEngine->waylandWindow.pBorderPixels[i][i2]
                              = PATACHE_BORDER_LOSTFOCUS_CSD_COLOR;
                        }
                    }

                  wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptorCSD,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                          * (pEngine->vulkan.swapchainExtent.height
                             + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                          * 4);

                  pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
                      pDecorationBorderPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                      (pEngine->vulkan.swapchainExtent.height + PATACHE_MAINBAR_HEIGHT_CSD_SIZE),
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4, WL_SHM_FORMAT_ARGB8888);

                  // Draw
                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                                     pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

                  wl_surface_damage_buffer (
                      pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                      (pEngine->vulkan.swapchainExtent.height + PATACHE_MAINBAR_HEIGHT_CSD_SIZE));

                  wl_shm_pool_destroy (pDecorationBorderPool);
                  close (borderFileDescriptorCSD);

                  wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
                }

              // Corner Border
              for (std::uint8_t i = 4; i < PATACHE_BORDER_CSD_SIZE; ++i)
                {
                  // File descriptor
                  char borderFileNameCSD[] = "Patache-Border";

                  std::int32_t borderFileDescriptorCSD
                      = shm_open (borderFileNameCSD, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

                  shm_unlink (borderFileNameCSD);

                  ftruncate (borderFileDescriptorCSD, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                          * 4);

                  // Memory
                  pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptorCSD, 0));

                  if (sFocusCSD)
                    {
                      for (std::uint32_t i2 = 0; i2 < (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                       * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
                           ++i2)
                        {
                          pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0; i2 < (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                       * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
                           ++i2)
                        {
                          pEngine->waylandWindow.pBorderPixels[i][i2]
                              = PATACHE_BORDER_LOSTFOCUS_CSD_COLOR;
                        }
                    }

                  wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
                      pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptorCSD,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                          * 4);

                  pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
                      pDecorationBorderPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                      PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4, WL_SHM_FORMAT_ARGB8888);

                  // Draw
                  wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                                     pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                            PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                            PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

                  wl_shm_pool_destroy (pDecorationBorderPool);
                  close (borderFileDescriptorCSD);

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
                  (pEngine->vulkan.swapchainExtent.width - 89), 2);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMaximize],
                  (pEngine->vulkan.swapchainExtent.width - 59), 2);

              wl_subsurface_set_position (
                  pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eClose],
                  (pEngine->vulkan.swapchainExtent.width - 29), 2);

              for (std::uint8_t i = 0; i < PATACHE_BUTTON_CSD_SIZE; ++i)
                {
                  // File descriptor
                  char buttonFileNameCSD[] = "Patache-Button-File";

                  std::int32_t buttonFileDescriptorCSD
                      = shm_open (buttonFileNameCSD, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

                  shm_unlink (buttonFileNameCSD);

                  ftruncate (buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

                  // Memory
                  pEngine->waylandWindow.pDecorationButtonPixels[i] = static_cast<std::uint32_t *> (
                      mmap (nullptr, PATACHE_CLOSE_BUTTON_CSD_SIZE, PROT_READ | PROT_WRITE,
                            MAP_SHARED, buttonFileDescriptorCSD, 0));

                  if (sFocusCSD)
                    {
                      // Focus Colors
                      switch (i)
                        {
                        case Patache::ButtonIndexCSD::eMinimize:
                          for (std::uint32_t i2 = 0; i2 < PATACHE_MINIMIZE_BUTTON_CSD_PIXELCOUNT;
                               ++i2)
                            {
                              pEngine->waylandWindow
                                  .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i2]
                                  = sMinimizeButtonCSD[i2];
                            }
                          break;

                        case Patache::ButtonIndexCSD::eMaximize:
                          if (isMaximized)
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                                {
                                  pEngine->waylandWindow
                                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
                                                              [i2]
                                      = sMaximizedButtonCSD[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                                {
                                  pEngine->waylandWindow
                                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
                                                              [i2]
                                      = sMaximizeButtonCSD[i2];
                                }
                            }
                          break;

                        case Patache::ButtonIndexCSD::eClose:
                          for (std::uint32_t i2 = 0; i2 < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i2)
                            {
                              pEngine->waylandWindow
                                  .pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose][i2]
                                  = sCloseButtonCSD[i2];
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
                          for (std::uint32_t i2 = 0; i2 < PATACHE_MINIMIZE_BUTTON_CSD_PIXELCOUNT;
                               ++i2)
                            {
                              pEngine->waylandWindow
                                  .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i2]
                                  = sMinimizeButtonFocusLostCSD[i2];
                            }
                          break;

                        case Patache::ButtonIndexCSD::eMaximize:
                          if (isMaximized)
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                                {
                                  pEngine->waylandWindow
                                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
                                                              [i2]
                                      = sMaximizedButtonFocusLostCSD[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                                {
                                  pEngine->waylandWindow
                                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
                                                              [i2]
                                      = sMaximizeButtonFocusLostCSD[i2];
                                }
                            }
                          break;

                        case Patache::ButtonIndexCSD::eClose:
                          for (std::uint32_t i2 = 0; i2 < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i2)
                            {
                              pEngine->waylandWindow
                                  .pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose][i2]
                                  = sCloseButtonFocusLostCSD[i2];
                            }
                          break;
                        }
                    }

                  wl_shm_pool * pDecorationButtonPool
                      = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                            buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

                  pEngine->waylandWindow.pDecorationButtonBuffer[i] = wl_shm_pool_create_buffer (
                      pDecorationButtonPool, 0, PATACHE_CLOSE_BUTTON_CSD_WIDTH,
                      PATACHE_CLOSE_BUTTON_CSD_HEIGHT, PATACHE_CLOSE_BUTTON_CSD_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

                  // Draw
                  wl_surface_attach (pEngine->waylandWindow.pButtonSurface[i],
                                     pEngine->waylandWindow.pDecorationButtonBuffer[i], 0, 0);

                  wl_surface_damage_buffer (pEngine->waylandWindow.pButtonSurface[i], 0, 0,
                                            PATACHE_CLOSE_BUTTON_CSD_WIDTH,
                                            PATACHE_CLOSE_BUTTON_CSD_HEIGHT);

                  wl_shm_pool_destroy (pDecorationButtonPool);
                  close (buttonFileDescriptorCSD);

                  wl_surface_commit (pEngine->waylandWindow.pButtonSurface[i]);
                }

              /*
               * Main Bar
               * [||||||||||||||||||||||]
               */

              wl_subsurface_set_position (pEngine->waylandWindow.pMainBarSubSurface, 0,
                                          -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

              // File descriptor
              char MainBarName[] = "Patache-MainBar";

              std::int32_t MainBarFileDescriptor = shm_open (MainBarName, O_RDWR | O_CREAT | O_EXCL,
                                                             S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

              shm_unlink (MainBarName);

              ftruncate (MainBarFileDescriptor, pEngine->vulkan.swapchainExtent.width
                                                    * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4);

              // Memory
              pEngine->waylandWindow.pMainBarPixels = static_cast<std::uint32_t *> (
                  mmap (nullptr,
                        pEngine->vulkan.swapchainExtent.width * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4,
                        PROT_READ | PROT_WRITE, MAP_SHARED, MainBarFileDescriptor, 0));

              if (sFocusCSD)
                {
                  for (std::uint32_t i = 0; i < (pEngine->vulkan.swapchainExtent.width
                                                 * PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
                       ++i)
                    pEngine->waylandWindow.pMainBarPixels[i] = PATACHE_MAINBAR_FOCUS_CSD_COLOR;
                }
              else
                {
                  for (std::uint32_t i = 0; i < (pEngine->vulkan.swapchainExtent.width
                                                 * PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
                       ++i)
                    pEngine->waylandWindow.pMainBarPixels[i] = PATACHE_MAINBAR_LOSTFOCUS_CSD_COLOR;
                }

              wl_shm_pool * pDecorationMainBarPool = wl_shm_create_pool (
                  pEngine->waylandWindow.pDecorationSharedMemory, MainBarFileDescriptor,
                  pEngine->vulkan.swapchainExtent.width * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4);

              pEngine->waylandWindow.pMainBarBuffer = wl_shm_pool_create_buffer (
                  pDecorationMainBarPool, 0, pEngine->vulkan.swapchainExtent.width,
                  PATACHE_MAINBAR_HEIGHT_CSD_SIZE, pEngine->vulkan.swapchainExtent.width * 4,
                  WL_SHM_FORMAT_ARGB8888);

              // Draw
              wl_surface_attach (pEngine->waylandWindow.pMainBarSurface,
                                 pEngine->waylandWindow.pMainBarBuffer, 0, 0);
              wl_surface_damage_buffer (pEngine->waylandWindow.pMainBarSurface, 0, 0,
                                        pEngine->vulkan.swapchainExtent.width,
                                        PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

              wl_shm_pool_destroy (pDecorationMainBarPool);
              close (MainBarFileDescriptor);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }
        }
    }
}

static constexpr struct xdg_surface_listener sDesktopStyleUserInterfaceListener
    = { .configure = DesktopStyleUserInterfaceConfigure };

// Seat
// Using seat input for basic windowing needs
// cast to void to silence GNU Compiler
static void
PointerEnter (void * pData, wl_pointer * pPointer, std::uint32_t serial, wl_surface * pSurface,
              [[maybe_unused]] wl_fixed_t surfaceX, [[maybe_unused]] wl_fixed_t surfaceY)
{
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

  if (pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      spPointerSelectedSurface = pSurface;
      char cursorType[21]{ 0 };

      // Resize Cursor
      if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTop])
        std::strncpy (cursorType, "top_side", 20);
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottom])
        std::strncpy (cursorType, "bottom_side", 20);
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eLeft])
        std::strncpy (cursorType, "left_side", 20);
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopLeft])
        std::strncpy (cursorType, "top_left_corner", 20);
      else if (pSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomLeft])
        std::strncpy (cursorType, "bottom_left_corner", 20);
      else if (pSurface == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eRight])
        std::strncpy (cursorType, "right_side", 20);
      else if (pSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopRight])
        std::strncpy (cursorType, "top_right_corner", 20);
      else if (pSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomRight])
        std::strncpy (cursorType, "bottom_right_corner", 20);

      // Normal Cursor
      if (pSurface == pEngine->waylandWindow.pMainBarSurface
          || pSurface == pEngine->waylandWindow.pSurface
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize]
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]
          || pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
        {
          std::strncpy (cursorType, "left_ptr", 19);
        }

      wl_cursor * const pCursor
          = wl_cursor_theme_get_cursor (pEngine->waylandWindow.pCursorTheme, cursorType);

      if (pCursor != nullptr)
        {
          wl_cursor_image * const pCursorImage = pCursor->images[0];

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

          // File descriptor
          char buttonFileNameCSD[] = "Patache-Button-File";

          std::int32_t buttonFileDescriptorCSD = shm_open (
              buttonFileNameCSD, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

          shm_unlink (buttonFileNameCSD);

          ftruncate (buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
            {
              // Memory
              pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose]
                  = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_CLOSE_BUTTON_CSD_SIZE,
                                                        PROT_READ | PROT_WRITE, MAP_SHARED,
                                                        buttonFileDescriptorCSD, 0));

              for (std::uint32_t i = 0; i < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose][i]
                      = sCloseButtonHoverCSD[i];
                }

              wl_shm_pool * pDecorationButtonPool
                  = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                        buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eClose]
                  = wl_shm_pool_create_buffer (
                      pDecorationButtonPool, 0, PATACHE_CLOSE_BUTTON_CSD_WIDTH,
                      PATACHE_CLOSE_BUTTON_CSD_HEIGHT, PATACHE_CLOSE_BUTTON_CSD_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              // Draw
              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose],
                  pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eClose],
                  0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], 0, 0,
                  PATACHE_CLOSE_BUTTON_CSD_WIDTH, PATACHE_CLOSE_BUTTON_CSD_HEIGHT);

              wl_shm_pool_destroy (pDecorationButtonPool);
              close (buttonFileDescriptorCSD);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
            {
              // Memory
              pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
                  = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_MAXIMIZE_BUTTON_CSD_SIZE,
                                                        PROT_READ | PROT_WRITE, MAP_SHARED,
                                                        buttonFileDescriptorCSD, 0));

              if (isMaximized)
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizedButtonHoverCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizeButtonHoverCSD[i];
                    }
                }

              wl_shm_pool * pDecorationButtonPool
                  = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                        buttonFileDescriptorCSD, PATACHE_MAXIMIZE_BUTTON_CSD_SIZE);

              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMaximize]
                  = wl_shm_pool_create_buffer (
                      pDecorationButtonPool, 0, PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH,
                      PATACHE_MAXIMIZE_BUTTON_CSD_HEIGHT, PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              // Draw
              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize],
                  pEngine->waylandWindow
                      .pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMaximize],
                  0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], 0, 0,
                  PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH, PATACHE_MAXIMIZE_BUTTON_CSD_HEIGHT);

              wl_shm_pool_destroy (pDecorationButtonPool);
              close (buttonFileDescriptorCSD);

              wl_surface_commit (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]);

              wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
            }

          if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
            {
              // Memory
              pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize]
                  = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_MINIMIZE_BUTTON_CSD_SIZE,
                                                        PROT_READ | PROT_WRITE, MAP_SHARED,
                                                        buttonFileDescriptorCSD, 0));

              for (std::uint32_t i = 0; i < PATACHE_MINIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow
                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i]
                      = sMinimizeButtonHoverCSD[i];
                }

              wl_shm_pool * pDecorationButtonPool
                  = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                        buttonFileDescriptorCSD, PATACHE_MINIMIZE_BUTTON_CSD_SIZE);

              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMinimize]
                  = wl_shm_pool_create_buffer (
                      pDecorationButtonPool, 0, PATACHE_MINIMIZE_BUTTON_CSD_WIDTH,
                      PATACHE_MINIMIZE_BUTTON_CSD_HEIGHT, PATACHE_MINIMIZE_BUTTON_CSD_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              // Draw
              wl_surface_attach (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize],
                  pEngine->waylandWindow
                      .pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMinimize],
                  0, 0);

              wl_surface_damage_buffer (
                  pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], 0, 0,
                  PATACHE_MINIMIZE_BUTTON_CSD_WIDTH, PATACHE_MINIMIZE_BUTTON_CSD_HEIGHT);

              wl_shm_pool_destroy (pDecorationButtonPool);
              close (buttonFileDescriptorCSD);

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
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

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

      // File descriptor
      char buttonFileNameCSD[] = "Patache-Button-File";

      std::int32_t buttonFileDescriptorCSD = shm_open (buttonFileNameCSD, O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
      shm_unlink (buttonFileNameCSD);

      ftruncate (buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

      // Restore close button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose])
        {
          // Memory
          pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose]
              = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_CLOSE_BUTTON_CSD_SIZE,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                                    buttonFileDescriptorCSD, 0));

          if (sFocusCSD)
            {
              for (std::uint32_t i = 0; i < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose][i]
                      = sCloseButtonCSD[i];
                }
            }
          else
            {
              for (std::uint32_t i = 0; i < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eClose][i]
                      = sCloseButtonFocusLostCSD[i];
                }
            }

          wl_shm_pool * pDecorationButtonPool
              = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                    buttonFileDescriptorCSD, PATACHE_CLOSE_BUTTON_CSD_SIZE);

          pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eClose]
              = wl_shm_pool_create_buffer (pDecorationButtonPool, 0, PATACHE_CLOSE_BUTTON_CSD_WIDTH,
                                           PATACHE_CLOSE_BUTTON_CSD_HEIGHT,
                                           PATACHE_CLOSE_BUTTON_CSD_WIDTH * 4,
                                           WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose],
              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eClose], 0,
              0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose], 0, 0,
              PATACHE_CLOSE_BUTTON_CSD_WIDTH, PATACHE_CLOSE_BUTTON_CSD_HEIGHT);

          wl_shm_pool_destroy (pDecorationButtonPool);
          close (buttonFileDescriptorCSD);

          wl_surface_commit (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eClose]);

          wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
        }

      // Restore maximize button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize])
        {
          // Memory
          pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize]
              = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_MAXIMIZE_BUTTON_CSD_SIZE,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                                    buttonFileDescriptorCSD, 0));

          if (sFocusCSD)
            {
              if (isMaximized)
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizedButtonCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizeButtonCSD[i];
                    }
                }
            }
          else
            {
              if (isMaximized)
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizedButtonFocusLostCSD[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0; i < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                    {
                      pEngine->waylandWindow
                          .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i]
                          = sMaximizeButtonFocusLostCSD[i];
                    }
                }
            }

          wl_shm_pool * pDecorationButtonPool
              = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                    buttonFileDescriptorCSD, PATACHE_MAXIMIZE_BUTTON_CSD_SIZE);

          pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMaximize]
              = wl_shm_pool_create_buffer (
                  pDecorationButtonPool, 0, PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH,
                  PATACHE_MAXIMIZE_BUTTON_CSD_HEIGHT, PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH * 4,
                  WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize],
              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMaximize], 0,
              0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize], 0, 0,
              PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH, PATACHE_MAXIMIZE_BUTTON_CSD_HEIGHT);

          wl_shm_pool_destroy (pDecorationButtonPool);
          close (buttonFileDescriptorCSD);

          wl_surface_commit (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMaximize]);

          wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);
        }

      // Restore minimize button color back to normal or lost focus
      if (pSurface == pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize])
        {
          // Memory
          pEngine->waylandWindow.pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize]
              = static_cast<std::uint32_t *> (mmap (nullptr, PATACHE_MINIMIZE_BUTTON_CSD_SIZE,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                                    buttonFileDescriptorCSD, 0));

          if (sFocusCSD)
            {
              for (std::uint32_t i = 0; i < PATACHE_MINIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow
                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i]
                      = sMinimizeButtonCSD[i];
                }
            }
          else
            {
              for (std::uint32_t i = 0; i < PATACHE_MINIMIZE_BUTTON_CSD_PIXELCOUNT; ++i)
                {
                  pEngine->waylandWindow
                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i]
                      = sMinimizeButtonFocusLostCSD[i];
                }
            }

          wl_shm_pool * pDecorationButtonPool
              = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                    buttonFileDescriptorCSD, PATACHE_MINIMIZE_BUTTON_CSD_SIZE);

          pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMinimize]
              = wl_shm_pool_create_buffer (
                  pDecorationButtonPool, 0, PATACHE_MINIMIZE_BUTTON_CSD_WIDTH,
                  PATACHE_MINIMIZE_BUTTON_CSD_HEIGHT, PATACHE_MINIMIZE_BUTTON_CSD_WIDTH * 4,
                  WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize],
              pEngine->waylandWindow.pDecorationButtonBuffer[Patache::ButtonIndexCSD::eMinimize], 0,
              0);

          wl_surface_damage_buffer (
              pEngine->waylandWindow.pButtonSurface[Patache::ButtonIndexCSD::eMinimize], 0, 0,
              PATACHE_MAXIMIZE_BUTTON_CSD_WIDTH, PATACHE_MAXIMIZE_BUTTON_CSD_HEIGHT);

          wl_shm_pool_destroy (pDecorationButtonPool);
          close (buttonFileDescriptorCSD);

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
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

  // If Server Side Decorations is no available
  if (pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
    {
      if (spPointerSelectedSurface
          == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTop])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottom])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eRight])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eTopRight])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomLeft])
        {
          sResizeCSD = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        }
      else if (spPointerSelectedSurface
               == pEngine->waylandWindow.pBorderSurface[Patache::BorderIndexCSD::eBottomRight])
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
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

  // If Server Side Decorations is no available
  // Client window decoration interactions for button left of mouse
  if ((button == PATACHE_POINTER_LEFT_CSD_CODE) && (state == WL_POINTER_BUTTON_STATE_PRESSED)
      && !isFullScreen && pEngine->waylandWindow.pDecorationMananger == nullptr
      && pEngine->waylandWindow.pSubCompositor != nullptr)
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
              // Border Window
              for (std::uint8_t i = 0; i < PATACHE_BORDER_CSD_SIZE; ++i)
                {
                  pEngine->waylandWindow.pBorderSubSurface[i] = wl_subcompositor_get_subsurface (
                      pEngine->waylandWindow.pSubCompositor,
                      pEngine->waylandWindow.pBorderSurface[i], pEngine->waylandWindow.pSurface);
                }

              xdg_toplevel_unset_maximized (pEngine->waylandWindow.pDesktopWindow);
            }
          else
            {
              xdg_toplevel_set_maximized (pEngine->waylandWindow.pDesktopWindow);

              // Border Window
              for (std::uint8_t i = 0; i < PATACHE_BORDER_CSD_SIZE; ++i)
                {
                  wl_subsurface_destroy (pEngine->waylandWindow.pBorderSubSurface[i]);
                }
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

      if (spPointerSelectedSurface == pEngine->waylandWindow.pMainBarSurface && !isMaximized)
        {
          xdg_toplevel_move (pEngine->waylandWindow.pDesktopWindow, pEngine->waylandWindow.pInput,
                             serial);
        }
      else if (sResizeCSD != XDG_TOPLEVEL_RESIZE_EDGE_NONE)
        {
          xdg_toplevel_resize (pEngine->waylandWindow.pDesktopWindow, pEngine->waylandWindow.pInput,
                               serial, sResizeCSD);
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

static constexpr struct wl_pointer_listener sPointerListener = {
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
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

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

static constexpr struct wl_seat_listener sInputListener
    = { .capabilities = GetInputCapabilities, .name = Name };

// Registry
// Get Wayland handles from protocols and core
static void
AddObject (void * pData, wl_registry * pRegistry, std::uint32_t name, const char * pInterface,
           [[maybe_unused]] std::uint32_t version)
{
  Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);

  if (std::strcmp (pInterface, wl_compositor_interface.name) == 0)
    {
      pEngine->waylandWindow.pCompositor = static_cast<wl_compositor *> (
          wl_registry_bind (pRegistry, name, &wl_compositor_interface, 4));
    }
  else if (std::strcmp (pInterface, xdg_wm_base_interface.name) == 0)
    {
      pEngine->waylandWindow.pWindowManangerBase = static_cast<xdg_wm_base *> (
          wl_registry_bind (pRegistry, name, &xdg_wm_base_interface, 1));

      xdg_wm_base_add_listener (pEngine->waylandWindow.pWindowManangerBase,
                                &sWindowManangerBaseListener, nullptr);
    }
  else if (std::strcmp (pInterface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
      // Server Side Decoration (SSD)
      pEngine->waylandWindow.pDecorationMananger = static_cast<zxdg_decoration_manager_v1 *> (
          wl_registry_bind (pRegistry, name, &zxdg_decoration_manager_v1_interface, 1));
    }
  else if (std::strcmp (pInterface, wl_subcompositor_interface.name) == 0)
    {
      // Client Side Decoration (CSD)
      pEngine->waylandWindow.pSubCompositor = static_cast<wl_subcompositor *> (
          wl_registry_bind (pRegistry, name, &wl_subcompositor_interface, 1));
    }
  else if (std::strcmp (pInterface, wl_shm_interface.name) == 0)
    {
      pEngine->waylandWindow.pDecorationSharedMemory
          = static_cast<wl_shm *> (wl_registry_bind (pRegistry, name, &wl_shm_interface, 1));

      pEngine->waylandWindow.pCursorTheme
          = wl_cursor_theme_load (nullptr, 32, pEngine->waylandWindow.pDecorationSharedMemory);
    }
  else if (std::strcmp (pInterface, wl_seat_interface.name) == 0)
    {
      pEngine->waylandWindow.pInput
          = static_cast<wl_seat *> (wl_registry_bind (pRegistry, name, &wl_seat_interface, 1));

      wl_seat_add_listener (pEngine->waylandWindow.pInput, &sInputListener, pData);
    }
}

// This function is not used, but is here to silence GNU Compiler
static void
RemoveObject ([[maybe_unused]] void * pData, [[maybe_unused]] wl_registry * pRegistry,
              [[maybe_unused]] std::uint32_t name)
{
}

static constexpr struct wl_registry_listener sRegistryListener
    = { .global = AddObject, .global_remove = RemoveObject };

// xdg_toplevel
static void
GetWindowSize (void * pData, [[maybe_unused]] xdg_toplevel * pDesktopWindow, std::int32_t width,
               std::int32_t height, wl_array * pStates)
{
  // Resize vulkan swapchain
  if (width != 0 && height != 0)
    {
      Patache::Engine * pEngine = static_cast<Patache::Engine *> (pData);
      resizingPending           = true;

      // If Server Side Decorations (CSD) is no available
      if (pEngine->waylandWindow.pDecorationMananger == nullptr
          && pEngine->waylandWindow.pSubCompositor != nullptr)
        {
          /*
           * pStates
           * This is the worst part of wayland for C++
           */
          std::uint32_t * state = static_cast<std::uint32_t *> (pStates->data);
          std::uint32_t * end   = reinterpret_cast<std::uint32_t *> (
              static_cast<std::uint8_t *> (pStates->data) + pStates->size);

          sFocusCSD = false;
          for (; state < end; ++state)
            {
              if (*reinterpret_cast<enum xdg_toplevel_state *> (state)
                  == XDG_TOPLEVEL_STATE_ACTIVATED)
                sFocusCSD = true;
            }

          if (!isFullScreen && !isMaximized)
            {
              pEngine->vulkan.swapchainExtent.width = width - PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                      - PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE;

              pEngine->vulkan.swapchainExtent.height
                  = height - PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                    - (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2);
            }
          else if (isMaximized && !isFullScreen)
            {
              pEngine->vulkan.swapchainExtent.width = width;

              pEngine->vulkan.swapchainExtent.height = height - PATACHE_MAINBAR_HEIGHT_CSD_SIZE;
            }
          else
            {
              // CSD FullScreen
              pEngine->vulkan.swapchainExtent.width  = width;
              pEngine->vulkan.swapchainExtent.height = height;
            }
        }
      else
        {
          // Server Side Decoration (SSD)
          pEngine->vulkan.swapchainExtent.width  = width;
          pEngine->vulkan.swapchainExtent.height = height;
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

static constexpr struct xdg_toplevel_listener sDesktopWindowListener
    = { .configure        = GetWindowSize,
        .close            = CloseWindow,
        .configure_bounds = ConfigureBounds,
        .wm_capabilities  = WindowManangerCapabilities };
