#include "WaylandWindow.hpp"

bool
CreateWaylandWindow (const std::uint32_t & rWidth, const std::uint32_t & rHeight,
                     const char * const pWindowTitle, Patache::Engine * const pEngine)
{
  wl_registry * pRegistry = nullptr;

  // Display Connect
  pEngine->waylandWindow.pDisplay = wl_display_connect (nullptr);

  if (pEngine->waylandWindow.pDisplay == nullptr)
    {
      std::future<void> err
          = std::async (std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
                        "Cannot connect to wayland display. Use a wayland session",
                        std::cref (pEngine->configuration));

      return false;
    }

  // Display Get Registry
  pRegistry = wl_display_get_registry (pEngine->waylandWindow.pDisplay);

  if (pRegistry == nullptr)
    {
      std::future<void> err
          = std::async (std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
                        "Cannot get a registry from a display. Use a wayland session",
                        std::cref (pEngine->configuration));

      return false;
    }

  wl_registry_add_listener (pRegistry, &sRegistryListener, pEngine);

  wl_display_roundtrip (pEngine->waylandWindow.pDisplay);

  wl_registry_destroy (pRegistry);
  pRegistry = nullptr;

  // Errors
  if (pEngine->waylandWindow.pCompositor == nullptr)
    {
      std::future<void> err = std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
          "Missing wl_compositor in this compositor", std::cref (pEngine->configuration));

      return false;
    }
  else if (pEngine->waylandWindow.pWindowManangerBase == nullptr)
    {
      std::future<void> err = std::async (
          std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
          "Missing xdg_wm_base in this compositor", std::cref (pEngine->configuration));

      return false;
    }

  // Create Wayland Surface
  pEngine->waylandWindow.pSurface
      = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

  // Get Shell Surface
  pEngine->waylandWindow.pDesktopStyleUserInterface = xdg_wm_base_get_xdg_surface (
      pEngine->waylandWindow.pWindowManangerBase, pEngine->waylandWindow.pSurface);

  xdg_surface_add_listener (pEngine->waylandWindow.pDesktopStyleUserInterface,
                            &sDesktopStyleUserInterfaceListener, pEngine);

  // Get xdg_toplevel
  pEngine->waylandWindow.pDesktopWindow
      = xdg_surface_get_toplevel (pEngine->waylandWindow.pDesktopStyleUserInterface);

  pEngine->vulkan.swapchainExtent.width  = rWidth;
  pEngine->vulkan.swapchainExtent.height = rHeight;

  xdg_toplevel_set_min_size (pEngine->waylandWindow.pDesktopWindow, 640, 360);

  // Callback for Window events form the desktop
  xdg_toplevel_add_listener (pEngine->waylandWindow.pDesktopWindow, &sDesktopWindowListener,
                             pEngine);

  xdg_toplevel_set_title (pEngine->waylandWindow.pDesktopWindow, pWindowTitle);
  xdg_toplevel_set_app_id (pEngine->waylandWindow.pDesktopWindow, pWindowTitle);

  wl_surface_commit (pEngine->waylandWindow.pSurface);
  wl_display_roundtrip (pEngine->waylandWindow.pDisplay);
  wl_surface_commit (pEngine->waylandWindow.pSurface);

  // If Server Side Decorations is no available
  if (pEngine->waylandWindow.pDecorationMananger == nullptr)
    {
      if (pEngine->waylandWindow.pSubCompositor == nullptr)
        {
          std::future<void> err = std::async (std::launch::async, Patache::WarningMessage,
                                              "Missing wl_subcompositor in this compositor. "
                                              "continuing without CSD");

          goto SKIP_CSD_CREATION;
        }
      else if (pEngine->waylandWindow.pDecorationSharedMemory == nullptr)
        {
          std::future<void> err = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
              "Missing wl_shm in this compositor", std::cref (pEngine->configuration));

          return false;
        }
      else if (pEngine->waylandWindow.pInput == nullptr)
        {
          std::future<void> err = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache pEngine - Wayland",
              "Missing wl_seat in this compositor", std::cref (pEngine->configuration));

          return false;
        }

#if PATACHE_DEBUG == 1
      pEngine->debugInfo.windowDecorationType
          = Patache::WindowDecorationType::eClientSideDecoration;
#endif

      // Decoration
      /*
       * Main Bar
       * [||||||||||||||||||||||]
       */
      pEngine->waylandWindow.pMainBarSurface
          = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

      pEngine->waylandWindow.pMainBarSubSurface = wl_subcompositor_get_subsurface (
          pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pMainBarSurface,
          pEngine->waylandWindow.pSurface);

      wl_subsurface_set_position (pEngine->waylandWindow.pMainBarSubSurface, 0,
                                  -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      // File descriptor
      char MainBarName[] = "Patache-MainBar";

      std::int32_t MainBarFileDescriptor = shm_open (MainBarName, O_RDWR | O_CREAT | O_EXCL,
                                                     S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

      shm_unlink (MainBarName);

      ftruncate (MainBarFileDescriptor, rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4);

      // Draw
      pEngine->waylandWindow.pMainBarPixels = static_cast<std::uint32_t *> (
          mmap (nullptr, rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4, PROT_READ | PROT_WRITE,
                MAP_SHARED, MainBarFileDescriptor, 0));

      for (std::uint32_t i = 0; i < (rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE); ++i)
        {
          // Write Color for a wl_buffer
          pEngine->waylandWindow.pMainBarPixels[i] = PATACHE_MAINBAR_FOCUS_CSD_COLOR;
        }

      wl_shm_pool * pDecorationMainBarPool = wl_shm_create_pool (
          pEngine->waylandWindow.pDecorationSharedMemory, MainBarFileDescriptor,
          rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4);

      pEngine->waylandWindow.pMainBarBuffer = wl_shm_pool_create_buffer (
          pDecorationMainBarPool, 0, rWidth, PATACHE_MAINBAR_HEIGHT_CSD_SIZE, rWidth * 4,
          WL_SHM_FORMAT_ARGB8888);

      // Draw
      wl_surface_attach (pEngine->waylandWindow.pMainBarSurface,
                         pEngine->waylandWindow.pMainBarBuffer, 0, 0);
      wl_surface_damage_buffer (pEngine->waylandWindow.pMainBarSurface, 0, 0, rWidth,
                                PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      wl_shm_pool_destroy (pDecorationMainBarPool);
      close (MainBarFileDescriptor);

      wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);

      /*
       * Buttons
       * [MINIMIZE | MAXIMIZE | CLOSE]
       */
      for (std::uint8_t i = 0; i < 3; ++i)
        {
          pEngine->waylandWindow.pButtonSurface[i]
              = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

          pEngine->waylandWindow.pButtonSubSurface[i] = wl_subcompositor_get_subsurface (
              pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pButtonSurface[i],
              pEngine->waylandWindow.pMainBarSurface);
        }

      // Set button position
      wl_subsurface_set_position (
          pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMinimize],
          (rWidth - 89), 2);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMaximize],
          (rWidth - 59), 2);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eClose], (rWidth - 29),
          2);

      for (std::uint8_t i = 0; i < PATACHE_BUTTON_CSD_SIZE; ++i)
        {
          // File descriptor
          char CSDButtonFileName[] = "Patache-Button-File";

          std::int32_t CSDButtonFileDescriptor = shm_open (
              CSDButtonFileName, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

          shm_unlink (CSDButtonFileName);

          ftruncate (CSDButtonFileDescriptor, PATACHE_CLOSE_BUTTON_CSD_SIZE);

          // Memory
          pEngine->waylandWindow.pDecorationButtonPixels[i] = static_cast<std::uint32_t *> (
              mmap (nullptr, PATACHE_CLOSE_BUTTON_CSD_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                    CSDButtonFileDescriptor, 0));

          switch (i)
            {
            case Patache::ButtonIndexCSD::eMinimize:
              for (std::uint32_t i2 = 0; i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                {
                  pEngine->waylandWindow
                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMinimize][i2]
                      = sMinimizeButtonCSD[i2];
                }
              break;

            case Patache::ButtonIndexCSD::eMaximize:
              for (std::uint32_t i2 = 0; i2 < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i2)
                {
                  pEngine->waylandWindow
                      .pDecorationButtonPixels[Patache::ButtonIndexCSD::eMaximize][i2]
                      = sMaximizeButtonCSD[i2];
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

          wl_shm_pool * pDecorationButtonPool
              = wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                    CSDButtonFileDescriptor, PATACHE_CLOSE_BUTTON_CSD_SIZE);

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
          close (CSDButtonFileDescriptor);

          wl_surface_commit (pEngine->waylandWindow.pButtonSurface[i]);
        }

      // Border Window
      for (std::uint8_t i = 0; i < PATACHE_BORDER_CSD_SIZE; ++i)
        {
          pEngine->waylandWindow.pBorderSurface[i]
              = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

          pEngine->waylandWindow.pBorderSubSurface[i] = wl_subcompositor_get_subsurface (
              pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pBorderSurface[i],
              pEngine->waylandWindow.pSurface);
        }

      // Horizontal border
      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTop], 0, -26);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottom], 0, rHeight);

      // Vertical Border
      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eLeft],
          -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eRight], rWidth,
          -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      // Corner Borders
      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopLeft],
          -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, -26);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopRight], rWidth,
          -26);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomLeft],
          -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, rHeight);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomRight], rWidth,
          rHeight);

      // Horizontal border
      for (std::uint8_t i = 0; i < PATACHE_BORDER_HORIZONTAL_CSD_SIZE; ++i)
        {
          // File descriptor
          char CSDBorderFileName[] = "Patache-Border";

          std::int32_t CSDBorderFileDescriptor = shm_open (
              CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

          shm_unlink (CSDBorderFileName);

          ftruncate (CSDBorderFileDescriptor, rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4);

          // Memory
          pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (
              mmap (nullptr, rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4,
                    PROT_READ | PROT_WRITE, MAP_SHARED, CSDBorderFileDescriptor, 0));

          for (std::uint32_t i2 = 0; i2 < (rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE); ++i2)
            {
              pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
            }

          wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, CSDBorderFileDescriptor,
              rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4);

          pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
              pDecorationBorderPool, 0, rWidth, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, rWidth * 4,
              WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                             pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0, rWidth,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

          wl_shm_pool_destroy (pDecorationBorderPool);
          close (CSDBorderFileDescriptor);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Vertical Border
      for (std::uint8_t i = 2; i < PATACHE_BORDER_VERTICAL_CSD_SIZE; ++i)
        {
          // File descriptor
          char CSDBorderFileName[] = "Patache-Border";

          std::int32_t CSDBorderFileDescriptor = shm_open (
              CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

          shm_unlink (CSDBorderFileName);

          ftruncate (CSDBorderFileDescriptor, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                  * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                                                  * 4);

          // Memory
          pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (
              mmap (nullptr,
                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                        * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE) * 4,
                    PROT_READ | PROT_WRITE, MAP_SHARED, CSDBorderFileDescriptor, 0));

          for (std::uint32_t i2 = 0; i2 < PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                              * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
               ++i2)
            {
              pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
            }

          wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, CSDBorderFileDescriptor,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                  * 4);

          pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
              pDecorationBorderPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
              (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE),
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4, WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                             pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                    (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE));

          wl_shm_pool_destroy (pDecorationBorderPool);
          close (CSDBorderFileDescriptor);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Corner Border
      for (std::uint8_t i = 4; i < PATACHE_BORDER_CSD_SIZE; ++i)
        {
          // File descriptor
          char CSDBorderFileName[] = "Patache-Border";

          std::int32_t CSDBorderFileDescriptor = shm_open (
              CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);

          shm_unlink (CSDBorderFileName);

          ftruncate (CSDBorderFileDescriptor, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                  * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4);

          // Memory
          pEngine->waylandWindow.pBorderPixels[i] = static_cast<std::uint32_t *> (mmap (
              nullptr,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4,
              PROT_READ | PROT_WRITE, MAP_SHARED, CSDBorderFileDescriptor, 0));

          for (std::uint32_t i2 = 0;
               i2 < (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
               ++i2)
            {
              pEngine->waylandWindow.pBorderPixels[i][i2] = PATACHE_BORDER_CSD_COLOR;
            }

          wl_shm_pool * pDecorationBorderPool = wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, CSDBorderFileDescriptor,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4);

          pEngine->waylandWindow.pBorderBuffer[i] = wl_shm_pool_create_buffer (
              pDecorationBorderPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4,
              WL_SHM_FORMAT_ARGB8888);

          // Draw
          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i],
                             pEngine->waylandWindow.pBorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

          wl_shm_pool_destroy (pDecorationBorderPool);
          close (CSDBorderFileDescriptor);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }
    }
  else
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.windowDecorationType
          = Patache::WindowDecorationType::eServerSideDecoration;
#endif

      // Has server side decoration
      pEngine->waylandWindow.pDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration (
          pEngine->waylandWindow.pDecorationMananger, pEngine->waylandWindow.pDesktopWindow);

      zxdg_toplevel_decoration_v1_set_mode (pEngine->waylandWindow.pDecoration,
                                            ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    }

SKIP_CSD_CREATION:

  // SDL Window
  SDL_PropertiesID windowProperties = SDL_CreateProperties ();

  SDL_SetPointerProperty (SDL_GetGlobalProperties (),
                          SDL_PROP_GLOBAL_VIDEO_WAYLAND_WL_DISPLAY_POINTER,
                          pEngine->waylandWindow.pDisplay);

  SDL_SetPointerProperty (windowProperties, SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER,
                          pEngine->waylandWindow.pSurface);

  SDL_SetBooleanProperty (windowProperties, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);

  SDL_SetNumberProperty (windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, rWidth);

  SDL_SetNumberProperty (windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, rHeight);

  SDL_SetBooleanProperty (windowProperties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);

  SDL_SetBooleanProperty (windowProperties,
                          SDL_PROP_WINDOW_CREATE_WAYLAND_CREATE_EGL_WINDOW_BOOLEAN, false);

  SDL_SetBooleanProperty (windowProperties,
                          SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN, true);

  SDL_SetBooleanProperty (windowProperties,
                          SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, true);

  SDL_SetBooleanProperty (windowProperties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN,
                          true);

  pEngine->pGameWindow = SDL_CreateWindowWithProperties (windowProperties);

  if (pEngine->pGameWindow == nullptr)
    {
      std::future<void> err = std::async (std::launch::async, Patache::FatalErrorMessage,
                                          "Patache Engine - SDL & Wayland",
                                          "SDL Cannot import external surfaces. if you are "
                                          "using Gamescope try with --expose-wayland",
                                          std::cref (pEngine->configuration));

      return false;
    }

  return true;
}
