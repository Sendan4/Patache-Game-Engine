#include <future>
#include <functional>
#include <cstring>

#if defined(__linux__)
#include <sys/types.h>

#include <wayland-client.h>
#include <wayland-cursor.h>

// Wayland Protocols
#include <xdg-shell.h>
#include <xdg-decoration-unstable-v1.h>

#include <SDL3/SDL.h>
#include <fast_io.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "Log.hpp"

// Window Actions
bool        ResizingPending = false;
extern bool Resize;
extern bool IsFullScreen;

// CSD (Window Client Side Decoration) Actions
static bool         IsCSDFocus             = true;
bool                IsMaximized            = false;
static std::uint8_t CSDResize              = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
static wl_surface * PointerSelectedSurface = nullptr;
static SDL_Event    PushWaylandEvent{};
;

// CSD (Window Client Side Decoration) Pixel Data Buttons
// Minimize
#include "CSDMinimizeButtonLittleEndian.hpp"
#include "CSDMinimizeButtonHoverLittleEndian.hpp"
#include "CSDMinimizeButtonFocusLostLittleEndian.hpp"
// Maximize
#include "CSDMaximizeButtonLittleEndian.hpp"
#include "CSDMaximizeButtonHoverLittleEndian.hpp"
#include "CSDMaximizedButtonLittleEndian.hpp"
#include "CSDMaximizedButtonHoverLittleEndian.hpp"
#include "CSDMaximizeButtonFocusLostLittleEndian.hpp"
#include "CSDMaximizedButtonFocusLostLittleEndian.hpp"
// Close
#include "CSDCloseButtonLittleEndian.hpp"
#include "CSDCloseButtonHoverLittleEndian.hpp"
#include "CSDCloseButtonFocusLostLittleEndian.hpp"

// Sizes
#define PATACHE_CSD_HEIGHT_SIZE         22
#define PATACHE_CSD_THRESHOLD_EDGE_SIZE 4

// Color
#define PATACHE_CSD_FOCUS_COLOR            0xFFC6A982
#define PATACHE_CSD_LOSTFOCUS_COLOR        0xFF808080
#define PATACHE_CSD_BORDER_COLOR           0xFFF1E9E0
#define PATACHE_CSD_BORDER_LOSTFOCUS_COLOR 0xFFF2F2F2

// Mouse Buttons
#define PATACHE_CSD_POINTER_LEFT_CODE        272
#define PATACHE_CSD_POINTER_RIGHT_CODE       273
#define PATACHE_CSD_POINTER_WEEL_BUTTON_CODE 274

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

/*
 * xdg_surface
 * Here is where the Resize content and buffers work is done for the Wayland
 * Window
 */
static void
DesktopStyleUserInterfaceConfigure (void *        Data,
                                    xdg_surface * DesktopStyleUserInterface,
                                    std::uint32_t Serial)
{
  xdg_surface_ack_configure (DesktopStyleUserInterface, Serial);
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  if (ResizingPending)
    {
      Resize = true;

      SDL_SetWindowSize (Engine->GameWindow,
                         Engine->Vulkan.SwapChainExtent.width,
                         Engine->Vulkan.SwapChainExtent.height);

      // If Server Side Decorations is no available
      if (Engine->WaylandWindow.DecorationMananger == nullptr
          && Engine->WaylandWindow.SubCompositor != nullptr)
        {
          // Borders
          if (!IsMaximized && !IsFullScreen)
            {
              // Horizontal border
              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_TOP_BORDER_INDEX],
                  0, -26);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_BOTTOM_BORDER_INDEX],
                  0, Engine->Vulkan.SwapChainExtent.height);

              // Vertical Border
              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_LEFT_BORDER_INDEX],
                  -PATACHE_CSD_THRESHOLD_EDGE_SIZE, -PATACHE_CSD_HEIGHT_SIZE);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_RIGHT_BORDER_INDEX],
                  Engine->Vulkan.SwapChainExtent.width,
                  -PATACHE_CSD_HEIGHT_SIZE);

              // Corner Borders
              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_TOPLEFT_BORDER_INDEX],
                  -PATACHE_CSD_THRESHOLD_EDGE_SIZE, -26);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_TOPRIGHT_BORDER_INDEX],
                  Engine->Vulkan.SwapChainExtent.width, -26);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_BOTTOMLEFT_BORDER_INDEX],
                  -PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                  Engine->Vulkan.SwapChainExtent.height);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .BorderSubSurface[PATACHE_CSD_BOTTOMRIGHT_BORDER_INDEX],
                  Engine->Vulkan.SwapChainExtent.width,
                  Engine->Vulkan.SwapChainExtent.height);

              // Horizontal border
              for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_HORIZONTAL_SIZE;
                   ++i)
                {
                  // Create a file descriptor for a buffer
                  char         CSDBorderFileName[] = "Patache-Border";
                  std::int32_t CSDBorderFileDescriptor
                      = shm_open (CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
                  shm_unlink (CSDBorderFileName);
                  ftruncate (CSDBorderFileDescriptor,
                             Engine->Vulkan.SwapChainExtent.width
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

                  /*
                   * Map File to a memory (To write the color) to be used for a
                   * wl_buffer
                   */
                  Engine->WaylandWindow.BorderPixels[i]
                      = static_cast<std::uint32_t *> (
                          mmap (nullptr,
                                Engine->Vulkan.SwapChainExtent.width
                                    * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                CSDBorderFileDescriptor, 0));

                  // Write Color for a wl_buffer
                  if (IsCSDFocus)
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < (Engine->Vulkan.SwapChainExtent.width
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE);
                           ++i2)
                        {
                          Engine->WaylandWindow.BorderPixels[i][i2]
                              = PATACHE_CSD_BORDER_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < (Engine->Vulkan.SwapChainExtent.width
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE);
                           ++i2)
                        Engine->WaylandWindow.BorderPixels[i][i2]
                            = PATACHE_CSD_BORDER_LOSTFOCUS_COLOR;
                    }

                  wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
                      Engine->WaylandWindow.DecorationSharedMemory,
                      CSDBorderFileDescriptor,
                      Engine->Vulkan.SwapChainExtent.width
                          * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

                  Engine->WaylandWindow.BorderBuffer[i]
                      = wl_shm_pool_create_buffer (
                          DecorationBorderPool, 0,
                          Engine->Vulkan.SwapChainExtent.width,
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                          Engine->Vulkan.SwapChainExtent.width * 4,
                          WL_SHM_FORMAT_ARGB8888);

                  wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                                     Engine->WaylandWindow.BorderBuffer[i], 0,
                                     0);

                  wl_surface_damage_buffer (
                      Engine->WaylandWindow.BorderSurface[i], 0, 0,
                      Engine->Vulkan.SwapChainExtent.width,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE);

                  wl_shm_pool_destroy (DecorationBorderPool);
                  close (CSDBorderFileDescriptor);

                  wl_surface_commit (Engine->WaylandWindow.BorderSurface[i]);
                }

              // Vertical Border
              for (std::uint8_t i = 2; i < 4; ++i)
                {
                  // Create a file descriptor for a buffer
                  char         CSDBorderFileName[] = "Patache-Border";
                  std::int32_t CSDBorderFileDescriptor
                      = shm_open (CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
                  shm_unlink (CSDBorderFileName);
                  ftruncate (CSDBorderFileDescriptor,
                             PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                 * (Engine->Vulkan.SwapChainExtent.height
                                    + PATACHE_CSD_HEIGHT_SIZE)
                                 * 4);

                  /*
                   * Map File to a memory (To write the color) to be used for a
                   * wl_buffer
                   */
                  Engine->WaylandWindow.BorderPixels[i]
                      = static_cast<std::uint32_t *> (
                          mmap (nullptr,
                                PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                    * (Engine->Vulkan.SwapChainExtent.height
                                       + PATACHE_CSD_HEIGHT_SIZE)
                                    * 4,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                CSDBorderFileDescriptor, 0));

                  // Write Color for a wl_buffer
                  if (IsCSDFocus)
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                    * (Engine->Vulkan.SwapChainExtent.height
                                       + PATACHE_CSD_HEIGHT_SIZE);
                           ++i2)
                        {

                          Engine->WaylandWindow.BorderPixels[i][i2]
                              = PATACHE_CSD_BORDER_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                    * (Engine->Vulkan.SwapChainExtent.height
                                       + PATACHE_CSD_HEIGHT_SIZE);
                           ++i2)
                        Engine->WaylandWindow.BorderPixels[i][i2]
                            = PATACHE_CSD_BORDER_LOSTFOCUS_COLOR;
                    }

                  wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
                      Engine->WaylandWindow.DecorationSharedMemory,
                      CSDBorderFileDescriptor,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE
                          * (Engine->Vulkan.SwapChainExtent.height
                             + PATACHE_CSD_HEIGHT_SIZE)
                          * 4);

                  Engine->WaylandWindow.BorderBuffer[i]
                      = wl_shm_pool_create_buffer (
                          DecorationBorderPool, 0,
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                          (Engine->Vulkan.SwapChainExtent.height
                           + PATACHE_CSD_HEIGHT_SIZE),
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4,
                          WL_SHM_FORMAT_ARGB8888);

                  wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                                     Engine->WaylandWindow.BorderBuffer[i], 0,
                                     0);

                  wl_surface_damage_buffer (
                      Engine->WaylandWindow.BorderSurface[i], 0, 0,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                      (Engine->Vulkan.SwapChainExtent.height
                       + PATACHE_CSD_HEIGHT_SIZE));

                  wl_shm_pool_destroy (DecorationBorderPool);
                  close (CSDBorderFileDescriptor);

                  wl_surface_commit (Engine->WaylandWindow.BorderSurface[i]);
                }

              // Corner Border
              for (std::uint8_t i = 4; i < 8; ++i)
                {
                  // Create a file descriptor for a buffer
                  char         CSDBorderFileName[] = "Patache-Border";
                  std::int32_t CSDBorderFileDescriptor
                      = shm_open (CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
                  shm_unlink (CSDBorderFileName);
                  ftruncate (CSDBorderFileDescriptor,
                             PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

                  /*
                   * Map File to a memory (To write the color) to be used for a
                   * wl_buffer
                   */
                  Engine->WaylandWindow.BorderPixels[i]
                      = static_cast<std::uint32_t *> (
                          mmap (nullptr,
                                PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                    * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                CSDBorderFileDescriptor, 0));

                  // Write Color for a wl_buffer
                  if (IsCSDFocus)
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < (PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE);
                           ++i2)
                        {

                          Engine->WaylandWindow.BorderPixels[i][i2]
                              = PATACHE_CSD_BORDER_COLOR;
                        }
                    }
                  else
                    {
                      for (std::uint32_t i2 = 0;
                           i2 < (PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                 * PATACHE_CSD_THRESHOLD_EDGE_SIZE);
                           ++i2)
                        Engine->WaylandWindow.BorderPixels[i][i2]
                            = PATACHE_CSD_BORDER_LOSTFOCUS_COLOR;
                    }

                  wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
                      Engine->WaylandWindow.DecorationSharedMemory,
                      CSDBorderFileDescriptor,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE
                          * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

                  Engine->WaylandWindow.BorderBuffer[i]
                      = wl_shm_pool_create_buffer (
                          DecorationBorderPool, 0,
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                          PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4,
                          WL_SHM_FORMAT_ARGB8888);

                  wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                                     Engine->WaylandWindow.BorderBuffer[i], 0,
                                     0);

                  wl_surface_damage_buffer (
                      Engine->WaylandWindow.BorderSurface[i], 0, 0,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                      PATACHE_CSD_THRESHOLD_EDGE_SIZE);

                  wl_shm_pool_destroy (DecorationBorderPool);
                  close (CSDBorderFileDescriptor);

                  wl_surface_commit (Engine->WaylandWindow.BorderSurface[i]);
                }
            }

          if (!IsFullScreen)
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
                  Engine->WaylandWindow
                      .ButtonSubSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
                  (Engine->Vulkan.SwapChainExtent.width - 89), 2);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .ButtonSubSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
                  (Engine->Vulkan.SwapChainExtent.width - 59), 2);

              wl_subsurface_set_position (
                  Engine->WaylandWindow
                      .ButtonSubSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
                  (Engine->Vulkan.SwapChainExtent.width - 29), 2);

              for (std::uint8_t i = 0; i < 3; ++i)
                {
                  // Create a file descriptor for a buffer
                  char         CSDButtonFileName[] = "Patache-Button-File";
                  std::int32_t CSDButtonFileDescriptor
                      = shm_open (CSDButtonFileName, O_RDWR | O_CREAT | O_EXCL,
                                  S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
                  shm_unlink (CSDButtonFileName);
                  ftruncate (CSDButtonFileDescriptor,
                             PATACHE_CSD_CLOSE_BUTTON_SIZE);

                  /*
                   * Map File to a memory (To write the color) to be used for a
                   * wl_buffer
                   */
                  Engine->WaylandWindow.DecorationButtonPixels[i]
                      = static_cast<std::uint32_t *> (
                          mmap (nullptr, PATACHE_CSD_CLOSE_BUTTON_SIZE,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                CSDButtonFileDescriptor, 0));

                  if (IsCSDFocus)
                    {
                      // Focus Colors
                      switch (i)
                        {
                        case PATACHE_CSD_MINIMIZE_BUTTON_INDEX:
                          for (std::uint32_t i2 = 0;
                               i2 < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT;
                               ++i2)
                            {
                              Engine->WaylandWindow.DecorationButtonPixels
                                  [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i2]
                                  = CSDMinimizeButton[i2];
                            }
                          break;

                        case PATACHE_CSD_MAXIMIZE_BUTTON_INDEX:
                          if (IsMaximized)
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT;
                                   ++i2)
                                {
                                  Engine->WaylandWindow.DecorationButtonPixels
                                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i2]
                                      = CSDMaximizedButton[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT;
                                   ++i2)
                                {
                                  Engine->WaylandWindow.DecorationButtonPixels
                                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i2]
                                      = CSDMaximizeButton[i2];
                                }
                            }
                          break;

                        case PATACHE_CSD_CLOSE_BUTTON_INDEX:
                          for (std::uint32_t i2 = 0;
                               i2 < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i2)
                            {
                              Engine->WaylandWindow.DecorationButtonPixels
                                  [PATACHE_CSD_CLOSE_BUTTON_INDEX][i2]
                                  = CSDCloseButton[i2];
                            }
                          break;
                        }
                    }
                  else
                    {
                      // Lost Focus colors
                      switch (i)
                        {
                        case PATACHE_CSD_MINIMIZE_BUTTON_INDEX:
                          for (std::uint32_t i2 = 0;
                               i2 < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT;
                               ++i2)
                            {
                              Engine->WaylandWindow.DecorationButtonPixels
                                  [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i2]
                                  = CSDMinimizeButtonFocusLost[i2];
                            }
                          break;

                        case PATACHE_CSD_MAXIMIZE_BUTTON_INDEX:
                          if (IsMaximized)
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT;
                                   ++i2)
                                {
                                  Engine->WaylandWindow.DecorationButtonPixels
                                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i2]
                                      = CSDMaximizedButtonFocusLost[i2];
                                }
                            }
                          else
                            {
                              for (std::uint32_t i2 = 0;
                                   i2 < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT;
                                   ++i2)
                                {
                                  Engine->WaylandWindow.DecorationButtonPixels
                                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i2]
                                      = CSDMaximizeButtonFocusLost[i2];
                                }
                            }
                          break;

                        case PATACHE_CSD_CLOSE_BUTTON_INDEX:
                          for (std::uint32_t i2 = 0;
                               i2 < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i2)
                            {
                              Engine->WaylandWindow.DecorationButtonPixels
                                  [PATACHE_CSD_CLOSE_BUTTON_INDEX][i2]
                                  = CSDCloseButtonFocusLost[i2];
                            }
                          break;
                        }
                    }

                  wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
                      Engine->WaylandWindow.DecorationSharedMemory,
                      CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

                  Engine->WaylandWindow.DecorationButtonBuffer[i]
                      = wl_shm_pool_create_buffer (
                          DecorationButtonPool, 0,
                          PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                          PATACHE_CSD_CLOSE_BUTTON_HEIGHT,
                          PATACHE_CSD_CLOSE_BUTTON_WIDTH * 4,
                          WL_SHM_FORMAT_ARGB8888);

                  wl_surface_attach (
                      Engine->WaylandWindow.ButtonSurface[i],
                      Engine->WaylandWindow.DecorationButtonBuffer[i], 0, 0);

                  wl_surface_damage_buffer (
                      Engine->WaylandWindow.ButtonSurface[i], 0, 0,
                      PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                      PATACHE_CSD_CLOSE_BUTTON_HEIGHT);

                  wl_shm_pool_destroy (DecorationButtonPool);
                  close (CSDButtonFileDescriptor);

                  wl_surface_commit (Engine->WaylandWindow.ButtonSurface[i]);
                }

              /*
               * Main Bar
               * [||||||||||||||||||||||]
               */

              wl_subsurface_set_position (
                  Engine->WaylandWindow.MainBarSubSurface, 0,
                  -PATACHE_CSD_HEIGHT_SIZE);

              // Create a file descriptor for a buffer
              char         MainBarName[] = "Patache-MainBar";
              std::int32_t MainBarFileDescriptor
                  = shm_open (MainBarName, O_RDWR | O_CREAT | O_EXCL,
                              S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
              shm_unlink (MainBarName);
              ftruncate (MainBarFileDescriptor,
                         Engine->Vulkan.SwapChainExtent.width
                             * PATACHE_CSD_HEIGHT_SIZE * 4);

              /*
               * Map File to a memory (To write the color) to be used for a
               * wl_buffer
               */
              Engine->WaylandWindow.MainBarPixels
                  = static_cast<std::uint32_t *> (
                      mmap (nullptr,
                            Engine->Vulkan.SwapChainExtent.width
                                * PATACHE_CSD_HEIGHT_SIZE * 4,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            MainBarFileDescriptor, 0));

              // Write Color for a wl_buffer
              if (IsCSDFocus)
                {
                  for (std::uint32_t i = 0;
                       i < (Engine->Vulkan.SwapChainExtent.width
                            * PATACHE_CSD_HEIGHT_SIZE);
                       ++i)
                    Engine->WaylandWindow.MainBarPixels[i]
                        = PATACHE_CSD_FOCUS_COLOR;
                }
              else
                {
                  for (std::uint32_t i = 0;
                       i < (Engine->Vulkan.SwapChainExtent.width
                            * PATACHE_CSD_HEIGHT_SIZE);
                       ++i)
                    Engine->WaylandWindow.MainBarPixels[i]
                        = PATACHE_CSD_LOSTFOCUS_COLOR;
                }

              wl_shm_pool * DecorationPool = wl_shm_create_pool (
                  Engine->WaylandWindow.DecorationSharedMemory,
                  MainBarFileDescriptor,
                  Engine->Vulkan.SwapChainExtent.width
                      * PATACHE_CSD_HEIGHT_SIZE * 4);

              Engine->WaylandWindow.MainBarBuffer = wl_shm_pool_create_buffer (
                  DecorationPool, 0, Engine->Vulkan.SwapChainExtent.width,
                  PATACHE_CSD_HEIGHT_SIZE,
                  Engine->Vulkan.SwapChainExtent.width * 4,
                  WL_SHM_FORMAT_ARGB8888);

              wl_surface_attach (Engine->WaylandWindow.MainBarSurface,
                                 Engine->WaylandWindow.MainBarBuffer, 0, 0);
              wl_surface_damage_buffer (Engine->WaylandWindow.MainBarSurface,
                                        0, 0,
                                        Engine->Vulkan.SwapChainExtent.width,
                                        PATACHE_CSD_HEIGHT_SIZE);

              wl_shm_pool_destroy (DecorationPool);
              close (MainBarFileDescriptor);

              wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
            }
        }
    }
}

static const struct xdg_surface_listener DesktopStyleUserInterfaceListener
    = { .configure = DesktopStyleUserInterfaceConfigure };

// Seat
// Using seat input for basic windowing needs
// cast to void to silence GNU Compiler
static void
PointerEnter (void * Data, wl_pointer * Pointer, std::uint32_t Serial,
              wl_surface * Surface, wl_fixed_t SurfaceX, wl_fixed_t SurfaceY)
{
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  static_cast<void> (SurfaceX);
  static_cast<void> (SurfaceY);

  if (Engine->WaylandWindow.DecorationMananger == nullptr
      && Engine->WaylandWindow.SubCompositor != nullptr)
    {
      PointerSelectedSurface = Surface;
      char CursorType[21]{ 0 };

      // Resize Cursor
      if (Surface
          == Engine->WaylandWindow.BorderSurface[PATACHE_CSD_TOP_BORDER_INDEX])
        std::strncpy (CursorType, "top_side", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOM_BORDER_INDEX])
        std::strncpy (CursorType, "bottom_side", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_LEFT_BORDER_INDEX])
        std::strncpy (CursorType, "left_side", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_TOPLEFT_BORDER_INDEX])
        std::strncpy (CursorType, "top_left_corner", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOMLEFT_BORDER_INDEX])
        std::strncpy (CursorType, "bottom_left_corner", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_RIGHT_BORDER_INDEX])
        std::strncpy (CursorType, "right_side", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_TOPRIGHT_BORDER_INDEX])
        std::strncpy (CursorType, "top_right_corner", 20);
      else if (Surface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOMRIGHT_BORDER_INDEX])
        std::strncpy (CursorType, "bottom_right_corner", 20);

      // Normal Cursor
      if (Surface == Engine->WaylandWindow.MainBarSurface
          || Surface == Engine->WaylandWindow.Surface
          || Surface
                 == Engine->WaylandWindow
                        .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]
          || Surface
                 == Engine->WaylandWindow
                        .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]
          || Surface
                 == Engine->WaylandWindow
                        .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX])
        {
          std::strncpy (CursorType, "left_ptr", 19);
        }

      wl_cursor * const Cursor = wl_cursor_theme_get_cursor (
          Engine->WaylandWindow.CursorTheme, CursorType);

      if (Cursor != nullptr)
        {
          wl_cursor_image * const CursorImage = Cursor->images[0];

          wl_pointer_set_cursor (
              Pointer, Serial, Engine->WaylandWindow.CursorSurface,
              CursorImage->hotspot_x, CursorImage->hotspot_y);

          wl_surface_attach (Engine->WaylandWindow.CursorSurface,
                             wl_cursor_image_get_buffer (CursorImage), 0, 0);
          wl_surface_damage (Engine->WaylandWindow.CursorSurface, 0, 0,
                             CursorImage->width, CursorImage->height);
          wl_surface_commit (Engine->WaylandWindow.CursorSurface);
        }

      // Button Hover
      if (!IsFullScreen)
        {
          /*
           * Buttons
           * [MINIMIZE | MAXIMIZE | CLOSE]
           *
           * Recreate the 3 buttons before the main bar for their have the
           * correct position in the main bar
           */

          // Create a file descriptor for a buffer
          char         CSDButtonFileName[] = "Patache-Button-File";
          std::int32_t CSDButtonFileDescriptor
              = shm_open (CSDButtonFileName, O_RDWR | O_CREAT | O_EXCL,
                          S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
          shm_unlink (CSDButtonFileName);
          ftruncate (CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

          if (Surface
              == Engine->WaylandWindow
                     .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX])
            {
              /*
               * Map File to a memory (To write the color) to be used for a
               * wl_buffer
               */
              Engine->WaylandWindow
                  .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                  = static_cast<std::uint32_t *> (
                      mmap (nullptr, PATACHE_CSD_CLOSE_BUTTON_SIZE,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            CSDButtonFileDescriptor, 0));

              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow
                      .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                                             [i]
                      = CSDCloseButtonHover[i];
                }

              wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
                  Engine->WaylandWindow.DecorationSharedMemory,
                  CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                  = wl_shm_pool_create_buffer (
                      DecorationButtonPool, 0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                      PATACHE_CSD_CLOSE_BUTTON_HEIGHT,
                      PATACHE_CSD_CLOSE_BUTTON_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              wl_surface_attach (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
                  Engine->WaylandWindow
                      .DecorationButtonBuffer[PATACHE_CSD_CLOSE_BUTTON_INDEX],
                  0, 0);

              wl_surface_damage_buffer (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
                  0, 0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                  PATACHE_CSD_CLOSE_BUTTON_HEIGHT);

              wl_shm_pool_destroy (DecorationButtonPool);
              close (CSDButtonFileDescriptor);

              wl_surface_commit (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX]);

              wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
            }

          if (Surface
              == Engine->WaylandWindow
                     .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX])
            {
              /*
               * Map File to a memory (To write the color) to be used for a
               * wl_buffer
               */
              Engine->WaylandWindow
                  .DecorationButtonPixels[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]
                  = static_cast<std::uint32_t *> (
                      mmap (nullptr, PATACHE_CSD_MAXIMIZE_BUTTON_SIZE,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            CSDButtonFileDescriptor, 0));

              if (IsMaximized)
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizedButtonHover[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizeButtonHover[i];
                    }
                }

              wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
                  Engine->WaylandWindow.DecorationSharedMemory,
                  CSDButtonFileDescriptor, PATACHE_CSD_MAXIMIZE_BUTTON_SIZE);

              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]
                  = wl_shm_pool_create_buffer (
                      DecorationButtonPool, 0,
                      PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH,
                      PATACHE_CSD_MAXIMIZE_BUTTON_HEIGHT,
                      PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              wl_surface_attach (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
                  Engine->WaylandWindow.DecorationButtonBuffer
                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
                  0, 0);

              wl_surface_damage_buffer (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
                  0, 0, PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH,
                  PATACHE_CSD_MAXIMIZE_BUTTON_HEIGHT);

              wl_shm_pool_destroy (DecorationButtonPool);
              close (CSDButtonFileDescriptor);

              wl_surface_commit (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]);

              wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
            }

          if (Surface
              == Engine->WaylandWindow
                     .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX])
            {
              /*
               * Map File to a memory (To write the color) to be used for a
               * wl_buffer
               */
              Engine->WaylandWindow
                  .DecorationButtonPixels[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]
                  = static_cast<std::uint32_t *> (
                      mmap (nullptr, PATACHE_CSD_MINIMIZE_BUTTON_SIZE,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            CSDButtonFileDescriptor, 0));

              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow.DecorationButtonPixels
                      [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i]
                      = CSDMinimizeButtonHover[i];
                }

              wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
                  Engine->WaylandWindow.DecorationSharedMemory,
                  CSDButtonFileDescriptor, PATACHE_CSD_MINIMIZE_BUTTON_SIZE);

              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]
                  = wl_shm_pool_create_buffer (
                      DecorationButtonPool, 0,
                      PATACHE_CSD_MINIMIZE_BUTTON_WIDTH,
                      PATACHE_CSD_MINIMIZE_BUTTON_HEIGHT,
                      PATACHE_CSD_MINIMIZE_BUTTON_WIDTH * 4,
                      WL_SHM_FORMAT_ARGB8888);

              wl_surface_attach (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
                  Engine->WaylandWindow.DecorationButtonBuffer
                      [PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
                  0, 0);

              wl_surface_damage_buffer (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
                  0, 0, PATACHE_CSD_MINIMIZE_BUTTON_WIDTH,
                  PATACHE_CSD_MINIMIZE_BUTTON_HEIGHT);

              wl_shm_pool_destroy (DecorationButtonPool);
              close (CSDButtonFileDescriptor);

              wl_surface_commit (
                  Engine->WaylandWindow
                      .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]);

              wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
            }
        }
    }
}

static void
PointerLeave (void * Data, wl_pointer * Pointer, uint Serial,
              wl_surface * Surface)
{
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  static_cast<void> (Pointer);
  static_cast<void> (Serial);

  // Restore button colors when the cursor leave the surface of that button
  if (!IsFullScreen && Engine->WaylandWindow.DecorationMananger == nullptr
      && Engine->WaylandWindow.SubCompositor != nullptr)
    {
      /*
       * Buttons
       * [MINIMIZE | MAXIMIZE | CLOSE]
       *
       * Recreate the 3 buttons before the main bar for their have the
       * correct position in the main bar
       */

      // Create a file descriptor for a buffer
      char         CSDButtonFileName[] = "Patache-Button-File";
      std::int32_t CSDButtonFileDescriptor
          = shm_open (CSDButtonFileName, O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
      shm_unlink (CSDButtonFileName);
      ftruncate (CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

      // Restore close button color back to normal or lost focus
      if (Surface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX])
        {
          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow
              .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr, PATACHE_CSD_CLOSE_BUTTON_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDButtonFileDescriptor, 0));

          if (IsCSDFocus)
            {
              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow
                      .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                                             [i]
                      = CSDCloseButton[i];
                }
            }
          else
            {
              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow
                      .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                                             [i]
                      = CSDCloseButtonFocusLost[i];
                }
            }

          wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

          Engine->WaylandWindow
              .DecorationButtonBuffer[PATACHE_CSD_CLOSE_BUTTON_INDEX]
              = wl_shm_pool_create_buffer (
                  DecorationButtonPool, 0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                  PATACHE_CSD_CLOSE_BUTTON_HEIGHT,
                  PATACHE_CSD_CLOSE_BUTTON_WIDTH * 4, WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_CLOSE_BUTTON_INDEX],
              0, 0);

          wl_surface_damage_buffer (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
              0, 0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
              PATACHE_CSD_CLOSE_BUTTON_HEIGHT);

          wl_shm_pool_destroy (DecorationButtonPool);
          close (CSDButtonFileDescriptor);

          wl_surface_commit (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX]);

          wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
        }

      // Restore maximize button color back to normal or lost focus
      if (Surface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX])
        {
          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow
              .DecorationButtonPixels[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr, PATACHE_CSD_MAXIMIZE_BUTTON_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDButtonFileDescriptor, 0));

          if (IsCSDFocus)
            {
              if (IsMaximized)
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizedButton[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizeButton[i];
                    }
                }
            }
          else
            {
              if (IsMaximized)
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizedButtonFocusLost[i];
                    }
                }
              else
                {
                  for (std::uint32_t i = 0;
                       i < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i)
                    {
                      Engine->WaylandWindow.DecorationButtonPixels
                          [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i]
                          = CSDMaximizeButtonFocusLost[i];
                    }
                }
            }

          wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDButtonFileDescriptor, PATACHE_CSD_MAXIMIZE_BUTTON_SIZE);

          Engine->WaylandWindow
              .DecorationButtonBuffer[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]
              = wl_shm_pool_create_buffer (
                  DecorationButtonPool, 0, PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH,
                  PATACHE_CSD_MAXIMIZE_BUTTON_HEIGHT,
                  PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH * 4,
                  WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
              0, 0);

          wl_surface_damage_buffer (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
              0, 0, PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH,
              PATACHE_CSD_MAXIMIZE_BUTTON_HEIGHT);

          wl_shm_pool_destroy (DecorationButtonPool);
          close (CSDButtonFileDescriptor);

          wl_surface_commit (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX]);

          wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
        }

      // Restore minimize button color back to normal or lost focus
      if (Surface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX])
        {
          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow
              .DecorationButtonPixels[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr, PATACHE_CSD_MINIMIZE_BUTTON_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDButtonFileDescriptor, 0));

          if (IsCSDFocus)
            {
              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow.DecorationButtonPixels
                      [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i]
                      = CSDMinimizeButton[i];
                }
            }
          else
            {
              for (std::uint32_t i = 0;
                   i < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT; ++i)
                {
                  Engine->WaylandWindow.DecorationButtonPixels
                      [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i]
                      = CSDMinimizeButtonFocusLost[i];
                }
            }

          wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDButtonFileDescriptor, PATACHE_CSD_MINIMIZE_BUTTON_SIZE);

          Engine->WaylandWindow
              .DecorationButtonBuffer[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]
              = wl_shm_pool_create_buffer (
                  DecorationButtonPool, 0, PATACHE_CSD_MINIMIZE_BUTTON_WIDTH,
                  PATACHE_CSD_MINIMIZE_BUTTON_HEIGHT,
                  PATACHE_CSD_MINIMIZE_BUTTON_WIDTH * 4,
                  WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
              Engine->WaylandWindow
                  .DecorationButtonBuffer[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
              0, 0);

          wl_surface_damage_buffer (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
              0, 0, PATACHE_CSD_MAXIMIZE_BUTTON_WIDTH,
              PATACHE_CSD_MAXIMIZE_BUTTON_HEIGHT);

          wl_shm_pool_destroy (DecorationButtonPool);
          close (CSDButtonFileDescriptor);

          wl_surface_commit (
              Engine->WaylandWindow
                  .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX]);

          wl_surface_commit (Engine->WaylandWindow.MainBarSurface);
        }
    }
}

static void
PointerMotion (void * Data, wl_pointer * Pointer, std::uint32_t Time,
               wl_fixed_t SurfaceX, wl_fixed_t SurfaceY)
{
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  static_cast<void> (Pointer);
  static_cast<void> (Time);
  static_cast<void> (SurfaceX);
  static_cast<void> (SurfaceY);

  // If Server Side Decorations is no available
  if (Engine->WaylandWindow.DecorationMananger == nullptr
      && Engine->WaylandWindow.SubCompositor != nullptr)
    {
      if (PointerSelectedSurface
          == Engine->WaylandWindow.BorderSurface[PATACHE_CSD_TOP_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOM_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_LEFT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_RIGHT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_TOPLEFT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_TOPRIGHT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOMLEFT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        }
      else if (PointerSelectedSurface
               == Engine->WaylandWindow
                      .BorderSurface[PATACHE_CSD_BOTTOMRIGHT_BORDER_INDEX])
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        }
      else
        {
          CSDResize = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        }
    }
}

static void
PointerPressButton (void * Data, wl_pointer * Pointer, std::uint32_t Serial,
                    std::uint32_t TimePressed, std::uint32_t Button,
                    std::uint32_t State)
{
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  static_cast<void> (Pointer);
  static_cast<void> (TimePressed);

  // If Server Side Decorations is no available
  // Client window decoration interactions for button left of mouse
  if ((Button == PATACHE_CSD_POINTER_LEFT_CODE)
      && (State == WL_POINTER_BUTTON_STATE_PRESSED) && !IsFullScreen
      && Engine->WaylandWindow.DecorationMananger == nullptr
      && Engine->WaylandWindow.SubCompositor != nullptr)
    {
      // Minimize Button
      if (PointerSelectedSurface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX])
        {
          xdg_toplevel_set_minimized (Engine->WaylandWindow.DesktopWindow);
        }

      // Maximize Button
      if (PointerSelectedSurface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX])
        {
          if (IsMaximized)
            {
              // Border Window
              for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_SIZE; ++i)
                {
                  Engine->WaylandWindow.BorderSubSurface[i]
                      = wl_subcompositor_get_subsurface (
                          Engine->WaylandWindow.SubCompositor,
                          Engine->WaylandWindow
                              .BorderSurface[i],          // surface turned
                                                          // into a sub-surface
                          Engine->WaylandWindow.Surface); // the parent surface
                }

              xdg_toplevel_unset_maximized (
                  Engine->WaylandWindow.DesktopWindow);
            }
          else
            {
              xdg_toplevel_set_maximized (Engine->WaylandWindow.DesktopWindow);

              // Border Window
              for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_SIZE; ++i)
                {
                  wl_subsurface_destroy (
                      Engine->WaylandWindow.BorderSubSurface[i]);
                }
            }

          IsMaximized ^= true;
        }

      // Close Button
      if (PointerSelectedSurface
          == Engine->WaylandWindow
                 .ButtonSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX])
        {
          PushWaylandEvent.type = SDL_EVENT_QUIT;
          SDL_PushEvent (&PushWaylandEvent);
        }

      if (PointerSelectedSurface == Engine->WaylandWindow.MainBarSurface
          && !IsMaximized)
        {
          xdg_toplevel_move (Engine->WaylandWindow.DesktopWindow,
                             Engine->WaylandWindow.Input, Serial);
        }
      else if (CSDResize != XDG_TOPLEVEL_RESIZE_EDGE_NONE)
        {
          xdg_toplevel_resize (Engine->WaylandWindow.DesktopWindow,
                               Engine->WaylandWindow.Input, Serial, CSDResize);
        }
    }
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
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  // If Server Side Decorations is no available
  // Using pointer input for client side window decorations (CSD)
  if (Capabilities & WL_SEAT_CAPABILITY_POINTER
      && Engine->WaylandWindow.DecorationMananger == nullptr
      && Engine->WaylandWindow.SubCompositor != nullptr)
    {
      wl_pointer_add_listener (wl_seat_get_pointer (Input), &PointerListener,
                               Data);

      Engine->WaylandWindow.CursorSurface
          = wl_compositor_create_surface (Engine->WaylandWindow.Compositor);
    }
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
  Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);

  if (strcmp (Interface, wl_compositor_interface.name) == 0)
    {
      Engine->WaylandWindow.Compositor = static_cast<wl_compositor *> (
          wl_registry_bind (Registry, Name, &wl_compositor_interface, 4));
    }
  else if (strcmp (Interface, xdg_wm_base_interface.name) == 0)
    {
      Engine->WaylandWindow.WindowManangerBase = static_cast<xdg_wm_base *> (
          wl_registry_bind (Registry, Name, &xdg_wm_base_interface, 1));

      xdg_wm_base_add_listener (Engine->WaylandWindow.WindowManangerBase,
                                &WindowManangerBaseListener, nullptr);
    }
  else if (strcmp (Interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
      // Server Side Decoration (SSD)
      Engine->WaylandWindow.DecorationMananger
          = static_cast<zxdg_decoration_manager_v1 *> (wl_registry_bind (
              Registry, Name, &zxdg_decoration_manager_v1_interface, 1));
    }
  else if (strcmp (Interface, wl_subcompositor_interface.name) == 0)
    {
      // Client Side Decoration (CSD)
      Engine->WaylandWindow.SubCompositor = static_cast<wl_subcompositor *> (
          wl_registry_bind (Registry, Name, &wl_subcompositor_interface, 1));
    }
  else if (strcmp (Interface, wl_shm_interface.name) == 0)
    {
      Engine->WaylandWindow.DecorationSharedMemory = static_cast<wl_shm *> (
          wl_registry_bind (Registry, Name, &wl_shm_interface, 1));

      Engine->WaylandWindow.CursorTheme = wl_cursor_theme_load (
          nullptr, 32, Engine->WaylandWindow.DecorationSharedMemory);
    }
  else if (strcmp (Interface, wl_seat_interface.name) == 0)
    {
      Engine->WaylandWindow.Input = static_cast<wl_seat *> (
          wl_registry_bind (Registry, Name, &wl_seat_interface, 1));

      wl_seat_add_listener (Engine->WaylandWindow.Input, &InputListener, Data);
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

  // Resize Vulkan SwapChain
  if (Width != 0 && Height != 0)
    {
      Patache::Engine * Engine = static_cast<Patache::Engine *> (Data);
      ResizingPending          = true;

      // If Server Side Decorations (CSD) is no available
      if (Engine->WaylandWindow.DecorationMananger == nullptr
          && Engine->WaylandWindow.SubCompositor != nullptr)
        {
          /*
           * States
           * This is the worst part of wayland for C++
           */
          std::uint32_t * State = static_cast<std::uint32_t *> (States->data);
          std::uint32_t * end   = reinterpret_cast<std::uint32_t *> (
              static_cast<std::uint8_t *> (States->data) + States->size);

          IsCSDFocus = false;
          for (; State < end; ++State)
            {
              if (*(enum xdg_toplevel_state *)State
                  == XDG_TOPLEVEL_STATE_ACTIVATED)
                IsCSDFocus = true;
            }

          if (!IsFullScreen && !IsMaximized)
            {
              Engine->Vulkan.SwapChainExtent.width
                  = Width - PATACHE_CSD_THRESHOLD_EDGE_SIZE
                    - PATACHE_CSD_THRESHOLD_EDGE_SIZE;

              Engine->Vulkan.SwapChainExtent.height
                  = Height - PATACHE_CSD_HEIGHT_SIZE
                    - (PATACHE_CSD_THRESHOLD_EDGE_SIZE * 2);
            }
          else if (IsMaximized && !IsFullScreen)
            {
              Engine->Vulkan.SwapChainExtent.width = Width;

              Engine->Vulkan.SwapChainExtent.height
                  = Height - PATACHE_CSD_HEIGHT_SIZE;
            }
          else
            {
              // CSD Fullscreen
              Engine->Vulkan.SwapChainExtent.width  = Width;
              Engine->Vulkan.SwapChainExtent.height = Height;
            }
        }
      else
        {
          // Server Side Decoration (SSD)
          Engine->Vulkan.SwapChainExtent.width  = Width;
          Engine->Vulkan.SwapChainExtent.height = Height;
        }
    }
}

static void
CloseWindow (void * Data, xdg_toplevel * DesktopWindow)
{
  static_cast<void> (Data);
  static_cast<void> (DesktopWindow);

  PushWaylandEvent.type = SDL_EVENT_QUIT;
  SDL_PushEvent (&PushWaylandEvent);
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
