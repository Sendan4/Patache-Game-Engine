#include "WaylandWindow.hpp"

bool
Patache::CreateWaylandWindow (const std::uint32_t & rWidth, const std::uint32_t & rHeight,
                              const char * const pWindowTitle, Patache::Engine * const pEngine)
{
  wl_registry * pRegistry{ nullptr };

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

  // Callback for Window events from the desktop
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
      xdg_surface_set_window_geometry (
          pEngine->waylandWindow.pDesktopStyleUserInterface, -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
          (-PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE - PATACHE_MAINBAR_HEIGHT_CSD_SIZE),
          (rWidth + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE),
          (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
           + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE));

      /*
       * Main Bar
       * [||||||||||||||||||||||]
       */

      std::random_device                           rd;
      std::default_random_engine                   fdGenerator (rd ());
      std::uniform_int_distribution<std::uint32_t> fdDist (0, UINT32_MAX);

      Patache::RandomizeFileDescriptorName (mainBarFileDescriptorName, 128U, fdDist, fdGenerator);

      pEngine->waylandWindow.pMainBarSurface
          = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

      pEngine->waylandWindow.pMainBarSubSurface = wl_subcompositor_get_subsurface (
          pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pMainBarSurface,
          pEngine->waylandWindow.pSurface);

      wl_subsurface_set_position (pEngine->waylandWindow.pMainBarSubSurface, 0,
                                  -PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      std::int32_t mainBarFileDescriptor{ shm_open (mainBarFileDescriptorName,
                                                    O_RDWR | O_CREAT | O_EXCL,
                                                    S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

      shm_unlink (mainBarFileDescriptorName);

      ftruncate (mainBarFileDescriptor, rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4U);

      std::uint32_t * pMainBarPixels{ static_cast<std::uint32_t *> (
          mmap (nullptr, rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4, PROT_READ | PROT_WRITE,
                MAP_SHARED, mainBarFileDescriptor, 0)) };

      for (std::uint32_t i{ 0U }; i < (rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE); ++i)
        {
          pMainBarPixels[i] = PATACHE_MAINBAR_FOCUS_CSD_COLOR;
        }

      wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                               mainBarFileDescriptor,
                                               rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4U) };

      wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
          pPool, 0, rWidth, PATACHE_MAINBAR_HEIGHT_CSD_SIZE, rWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

      wl_shm_pool_destroy (pPool);

      wl_surface_attach (pEngine->waylandWindow.pMainBarSurface, pBuffer, 0, 0);
      wl_surface_damage_buffer (pEngine->waylandWindow.pMainBarSurface, 0, 0, rWidth,
                                PATACHE_MAINBAR_HEIGHT_CSD_SIZE);

      Patache::SurfaceBufferCleanup * pCleanupMainBar{
        static_cast<Patache::SurfaceBufferCleanup *> (
            std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
      };

      pCleanupMainBar->mappedMemSize = rWidth * PATACHE_MAINBAR_HEIGHT_CSD_SIZE * 4;
      pCleanupMainBar->pMappedMem    = pMainBarPixels;
      pCleanupMainBar->fd            = mainBarFileDescriptor;

      wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanupMainBar);

      wl_surface_commit (pEngine->waylandWindow.pMainBarSurface);

      /*
       * Buttons
       * [MINIMIZE | MAXIMIZE | CLOSE]
       */
      for (std::uint8_t i{ 0U }; i < 3U; ++i)
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
          (rWidth - 87), 2);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eMaximize],
          (rWidth - 57), 2);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pButtonSubSurface[Patache::ButtonIndexCSD::eClose], (rWidth - 27),
          2);

      for (std::uint8_t i{ 0U }; i < PATACHE_BUTTON_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (buttonFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t buttonFileDescriptor{ shm_open (buttonFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (buttonFileDescriptorName[i]);

          ftruncate (buttonFileDescriptor, PATACHE_CLOSE_BUTTON_CSD_SIZE);

          std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, PATACHE_CLOSE_BUTTON_CSD_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buttonFileDescriptor, 0)) };

          switch (i)
            {
            case Patache::ButtonIndexCSD::eMinimize:
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_MAXIMIZE_BUTTON_CSD_PIXELCOUNT; ++i2)
                {
                  pButtonPixels[i2] = sMinimizeButtonCSD[i2];
                }
              break;

            case Patache::ButtonIndexCSD::eMaximize:
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i2)
                {

                  pButtonPixels[i2] = sMaximizeButtonCSD[i2];
                }
              break;

            case Patache::ButtonIndexCSD::eClose:
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_CLOSE_BUTTON_CSD_PIXELCOUNT; ++i2)
                {
                  pButtonPixels[i2] = sCloseButtonCSD[i2];
                }
              break;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                                   buttonFileDescriptor,
                                                   PATACHE_CLOSE_BUTTON_CSD_SIZE) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, PATACHE_CLOSE_BUTTON_CSD_WIDTH, PATACHE_CLOSE_BUTTON_CSD_HEIGHT,
              PATACHE_CLOSE_BUTTON_CSD_WIDTH * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pButtonSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pButtonSurface[i], 0, 0,
                                    PATACHE_CLOSE_BUTTON_CSD_WIDTH,
                                    PATACHE_CLOSE_BUTTON_CSD_HEIGHT);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = PATACHE_CLOSE_BUTTON_CSD_SIZE;
          pCleanupMainBar->pMappedMem    = pButtonPixels;
          pCleanupMainBar->fd            = buttonFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pButtonSurface[i]);
        }

      // Border Window
      for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
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
      for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_HORIZONTAL_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (borderFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U }; i2 < (rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE); ++i2)
            {
              pBorderPixels[i2] = PATACHE_BORDER_FOCUS_CSD_COLOR;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (pPool, 0, rWidth,
                                                          PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                                          rWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0, rWidth,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = rWidth * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U;
          pCleanupMainBar->pMappedMem    = pBorderPixels;
          pCleanupMainBar->fd            = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Vertical Border
      for (std::uint8_t i{ 2U }; i < PATACHE_BORDER_VERTICAL_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (borderFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                               * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE) * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                        * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE) * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U }; i2 < PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                                * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE);
               ++i2)
            {
              pBorderPixels[i2] = PATACHE_BORDER_FOCUS_CSD_COLOR;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE)
                  * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
              (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE),
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                    (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE));

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                           * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE) * 4U;
          pCleanupMainBar->pMappedMem    = pBorderPixels;
          pCleanupMainBar->fd            = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Corner Border
      for (std::uint8_t i{ 4U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (borderFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
                                               * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (mmap (
              nullptr,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U,
              PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U };
               i2 < (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);
               ++i2)
            {
              pBorderPixels[i2] = PATACHE_BORDER_FOCUS_CSD_COLOR;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
              PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U,
              WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
                                    PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize
              = PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 4U;
          pCleanupMainBar->pMappedMem = pBorderPixels;
          pCleanupMainBar->fd         = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Shadow
      for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_CSD_SIZE; ++i)
        {
          pEngine->waylandWindow.pShadowSurface[i]
              = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

          pEngine->waylandWindow.pShadowSubSurface[i] = wl_subcompositor_get_subsurface (
              pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pShadowSurface[i],
              pEngine->waylandWindow.pSurface);
        }

      // Horizontal Shadow
      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTop],
          -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE, PATACHE_SHADOW_HORIZONTAL_POSITION_Y);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottom],
          -PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE,
          (rHeight + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
           + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE));

      // Vertical Shadow
      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eLeft],
          PATACHE_SHADOW_VERTICAL_POSITION_X, PATACHE_SHADOW_VERTICAL_POSITION_Y);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eRight],
          (rWidth + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE), PATACHE_SHADOW_VERTICAL_POSITION_Y);

      // Corner Shadow
      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopLeft],
          PATACHE_SHADOW_VERTICAL_POSITION_X, PATACHE_SHADOW_HORIZONTAL_POSITION_Y);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopRight],
          (rWidth + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE), PATACHE_SHADOW_HORIZONTAL_POSITION_Y);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomLeft],
          PATACHE_SHADOW_VERTICAL_POSITION_X,
          (rHeight + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
           + PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE));

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomRight],
          (rWidth + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE),
          (rHeight + PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE
           + PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE));

      // Horizontal
      for (std::uint8_t i{ 0U }; i < PATACHE_BORDER_HORIZONTAL_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (shadowFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                                               * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                        * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eTop)
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                            * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE;
                   ++i2)
                {
                  if ((i2 % (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                           * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                       == 0)
                      && i3 < PATACHE_SHADOW_TOP_SIZE)
                    {
                      ++i3;
                    }

                  pBorderPixels[i2] = sShadowTop[i3];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                            * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE;
                   ++i2)
                {
                  if ((i2 % (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                           * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                       == 0)
                      && i3 < PATACHE_SHADOW_BOTTOM_SIZE)
                    {
                      ++i3;
                    }

                  pBorderPixels[i2] = sShadowBottom[i3];
                }
            }

          wl_shm_pool * pPoolTest{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
              (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                  * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE * 4U) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)),
              PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE,
              (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)) * 4U,
              WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)),
                                    PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE);

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = (rWidth + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                                          * PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE * 4U;
          pCleanupShadow->pMappedMem    = pBorderPixels;
          pCleanupShadow->fd            = shadowFileDescriptor;

          wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

          wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
        }

      // Vertical
      for (std::uint8_t i{ PATACHE_BORDER_HORIZONTAL_CSD_SIZE };
           i < PATACHE_BORDER_VERTICAL_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (shadowFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                                               * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                                                  + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                                               * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                        * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                           + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                        * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eRight)
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                         * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                            + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)));
                   ++i2, ++i3)
                {
                  if ((i2 % PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE == 0)
                      || i3 < PATACHE_SHADOW_RIGHT_SIZE)
                    {
                      i3 = 0;
                    }

                  pBorderPixels[i2] = sShadowRight[i3];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                         * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                            + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)));
                   ++i2, ++i3)
                {
                  if ((i2 % PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE == 0)
                      || i3 < PATACHE_SHADOW_LEFT_SIZE)
                    {
                      i3 = 0;
                    }

                  pBorderPixels[i2] = sShadowLeft[i3];
                }
            }

          wl_shm_pool * pPoolTest{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
              PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                  * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                     + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                  * 4U) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE,
              (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
               + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)),
              PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE,
                                    (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                                     + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2)));

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE
                                          * (rHeight + PATACHE_MAINBAR_HEIGHT_CSD_SIZE
                                             + (PATACHE_BORDER_THRESHOLDEDGE_CSD_SIZE * 2))
                                          * 4U;
          pCleanupShadow->pMappedMem    = pBorderPixels;
          pCleanupShadow->fd            = shadowFileDescriptor;

          wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

          wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
        }

      // Corner
      for (std::uint8_t i{ PATACHE_BORDER_VERTICAL_CSD_SIZE }; i < PATACHE_BORDER_CSD_SIZE; ++i)
        {
          Patache::RandomizeFileDescriptorName (shadowFileDescriptorName[i], 128U, fdDist,
                                                fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, PATACHE_SHADOW_CORNER_CSD_TOTALBYTESIZE);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, PATACHE_SHADOW_CORNER_CSD_TOTALBYTESIZE, PROT_READ | PROT_WRITE,
                    MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eTopLeft)
            {
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_SHADOW_CORNER_CSD_SIZE; ++i2)
                {
                  pBorderPixels[i2] = sShadowTopLeft[i2];
                }
            }
          else if (i == Patache::BorderIndexCSD::eTopRight)
            {
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_SHADOW_CORNER_CSD_SIZE; ++i2)
                {
                  pBorderPixels[i2] = sShadowTopRight[i2];
                }
            }
          else if (i == Patache::BorderIndexCSD::eBottomRight)
            {
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_SHADOW_CORNER_CSD_SIZE; ++i2)
                {
                  pBorderPixels[i2] = sShadowBottomRight[i2];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }; i2 < PATACHE_SHADOW_CORNER_CSD_SIZE; ++i2)
                {
                  pBorderPixels[i2] = sShadowBottomLeft[i2];
                }
            }

          wl_shm_pool * pPoolTest{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
              PATACHE_SHADOW_CORNER_CSD_TOTALBYTESIZE) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE,
              PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE, PATACHE_SHADOW_CORNER_WIDTH_CSD_BYTESIZE,
              WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE,
                                    PATACHE_SHADOW_THRESHOLDEDGE_CSD_SIZE);

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = PATACHE_SHADOW_CORNER_CSD_TOTALBYTESIZE;
          pCleanupShadow->pMappedMem    = pBorderPixels;
          pCleanupShadow->fd            = shadowFileDescriptor;

          wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

          wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
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
  SDL_PropertiesID windowProperties{ SDL_CreateProperties () };

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
