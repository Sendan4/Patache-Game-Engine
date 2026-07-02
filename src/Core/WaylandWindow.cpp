#include "Wayland.hpp"

bool
Patache::CreateWaylandWindow (const std::uint32_t & rWidth, const std::uint32_t & rHeight,
                              const char * const pWindowTitle, Patache::Engine * const pEngine)
{
  wl_registry * pRegistry{ nullptr };

  // Display Connect
  pEngine->waylandWindow.pDisplay = wl_display_connect (nullptr);

  if (pEngine->waylandWindow.pDisplay == nullptr)
    {
      Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                  "Cannot connect to wayland display. Use a wayland session",
                                  pEngine->configuration);

      return false;
    }

  // Display Get Registry
  pRegistry = wl_display_get_registry (pEngine->waylandWindow.pDisplay);

  if (pRegistry == nullptr)
    {
      Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                  "Cannot get a registry from a display. Use a wayland session",
                                  pEngine->configuration);

      return false;
    }

  wl_registry_add_listener (pRegistry, &sRegistryListener, pEngine);

  wl_display_roundtrip (pEngine->waylandWindow.pDisplay);

  wl_registry_destroy (pRegistry);
  pRegistry = nullptr;

  // Errors
  if (pEngine->waylandWindow.pCompositor == nullptr)
    {
      Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                  "Missing wl_compositor in this compositor",
                                  pEngine->configuration);

      return false;
    }
  else if (pEngine->waylandWindow.pWindowManangerBase == nullptr)
    {
      Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                  "Missing xdg_wm_base in this compositor", pEngine->configuration);

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
          Patache::WarningMessage ("Missing wl_subcompositor in this compositor. "
                                   "continuing without CSD");

          goto SKIP_CSD_CREATION;
        }
      else if (pEngine->waylandWindow.pDecorationSharedMemory == nullptr)
        {
          Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                      "Missing wl_shm in this compositor", pEngine->configuration);

          return false;
        }
      else if (pEngine->waylandWindow.pInput == nullptr)
        {
          Patache::FatalErrorMessage ("Patache pEngine - Wayland",
                                      "Missing wl_seat in this compositor", pEngine->configuration);

          return false;
        }

#if PATACHE_DEBUG == 1
      pEngine->debugInfo.windowDecorationType
          = Patache::WindowDecorationType::eClientSideDecoration;
#endif

      // Decoration
      xdg_surface_set_window_geometry (
          pEngine->waylandWindow.pDesktopStyleUserInterface, -Patache::sBorderThresholdEdgeCSDSize,
          (-Patache::sBorderThresholdEdgeCSDSize - Patache::sMainbarHeightCSDSize),
          (rWidth + Patache::sBorderThresholdEdgeCSDSize + Patache::sBorderThresholdEdgeCSDSize),
          (rHeight + Patache::sMainbarHeightCSDSize + Patache::sBorderThresholdEdgeCSDSize
           + Patache::sBorderThresholdEdgeCSDSize));

      /*
       * Main Bar
       * [||||||||||||||||||||||]
       */

      std::random_device                           rd;
      std::default_random_engine                   fdGenerator (rd ());
      std::uniform_int_distribution<std::uint32_t> fdDist (0, UINT32_MAX);

      Patache::RandomizeFileDescriptorName (mainBarFileDescriptorName,
                                            Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

      pEngine->waylandWindow.pMainBarSurface
          = wl_compositor_create_surface (pEngine->waylandWindow.pCompositor);

      pEngine->waylandWindow.pMainBarSubSurface = wl_subcompositor_get_subsurface (
          pEngine->waylandWindow.pSubCompositor, pEngine->waylandWindow.pMainBarSurface,
          pEngine->waylandWindow.pSurface);

      wl_subsurface_set_position (pEngine->waylandWindow.pMainBarSubSurface, 0,
                                  -Patache::sMainbarHeightCSDSize);

      std::int32_t mainBarFileDescriptor{ shm_open (mainBarFileDescriptorName,
                                                    O_RDWR | O_CREAT | O_EXCL,
                                                    S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

      shm_unlink (mainBarFileDescriptorName);

      ftruncate (mainBarFileDescriptor, rWidth * Patache::sMainbarHeightCSDSize * 4U);

      std::uint32_t * pMainBarPixels{ static_cast<std::uint32_t *> (
          mmap (nullptr, rWidth * Patache::sMainbarHeightCSDSize * 4, PROT_READ | PROT_WRITE,
                MAP_SHARED, mainBarFileDescriptor, 0)) };

      for (std::uint32_t i{ 0U }; i < (rWidth * Patache::sMainbarHeightCSDSize); ++i)
        {
          pMainBarPixels[i] = Patache::mainbarFocusCSDColor;
        }

      wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                               mainBarFileDescriptor,
                                               rWidth * Patache::sMainbarHeightCSDSize * 4U) };

      wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
          pPool, 0, rWidth, Patache::sMainbarHeightCSDSize, rWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

      wl_shm_pool_destroy (pPool);

      wl_surface_attach (pEngine->waylandWindow.pMainBarSurface, pBuffer, 0, 0);
      wl_surface_damage_buffer (pEngine->waylandWindow.pMainBarSurface, 0, 0, rWidth,
                                Patache::sMainbarHeightCSDSize);

      Patache::SurfaceBufferCleanup * pCleanupMainBar{
        static_cast<Patache::SurfaceBufferCleanup *> (
            std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
      };

      pCleanupMainBar->mappedMemSize = rWidth * Patache::sMainbarHeightCSDSize * 4;
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

      for (std::uint8_t i{ 0U }; i < Patache::sButtonCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              buttonFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t buttonFileDescriptor{ shm_open (buttonFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (buttonFileDescriptorName[i]);

          ftruncate (buttonFileDescriptor, Patache::sCloseButtonCSDSize);

          std::uint32_t * pButtonPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, Patache::sCloseButtonCSDSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buttonFileDescriptor, 0)) };

          switch (i)
            {
            case Patache::ButtonIndexCSD::eMinimize:
              for (std::uint32_t i2{ 0U }; i2 < Patache::sMaximizeButtonCSDPixelCount; ++i2)
                {
                  pButtonPixels[i2] = sMinimizeButtonCSD[i2];
                }
              break;

            case Patache::ButtonIndexCSD::eMaximize:
              for (std::uint32_t i2{ 0U }; i2 < Patache::sCloseButtonCSDPixelCount; ++i2)
                {

                  pButtonPixels[i2] = sMaximizeButtonCSD[i2];
                }
              break;

            case Patache::ButtonIndexCSD::eClose:
              for (std::uint32_t i2{ 0U }; i2 < Patache::sCloseButtonCSDPixelCount; ++i2)
                {
                  pButtonPixels[i2] = sCloseButtonCSD[i2];
                }
              break;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (pEngine->waylandWindow.pDecorationSharedMemory,
                                                   buttonFileDescriptor,
                                                   Patache::sCloseButtonCSDSize) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight,
              Patache::sCloseButtonCSDWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pButtonSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pButtonSurface[i], 0, 0,
                                    Patache::sCloseButtonCSDWidth, Patache::sCloseButtonCSDHeight);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = Patache::sCloseButtonCSDSize;
          pCleanupMainBar->pMappedMem    = pButtonPixels;
          pCleanupMainBar->fd            = buttonFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pButtonSurface[i]);
        }

      // Border Window
      for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
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
          -Patache::sBorderThresholdEdgeCSDSize, -Patache::sMainbarHeightCSDSize);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eRight], rWidth,
          -Patache::sMainbarHeightCSDSize);

      // Corner Borders
      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopLeft],
          -Patache::sBorderThresholdEdgeCSDSize, -26);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eTopRight], rWidth,
          -26);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomLeft],
          -Patache::sBorderThresholdEdgeCSDSize, rHeight);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pBorderSubSurface[Patache::BorderIndexCSD::eBottomRight], rWidth,
          rHeight);

      // Horizontal border
      for (std::uint8_t i{ 0U }; i < Patache::sBorderHorizontalCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              borderFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, rWidth * Patache::sBorderThresholdEdgeCSDSize * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, rWidth * Patache::sBorderThresholdEdgeCSDSize * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U }; i2 < (rWidth * Patache::sBorderThresholdEdgeCSDSize); ++i2)
            {
              pBorderPixels[i2] = Patache::borderFocusCSDColor;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              rWidth * Patache::sBorderThresholdEdgeCSDSize * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (pPool, 0, rWidth,
                                                          Patache::sBorderThresholdEdgeCSDSize,
                                                          rWidth * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0, rWidth,
                                    Patache::sBorderThresholdEdgeCSDSize);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = rWidth * Patache::sBorderThresholdEdgeCSDSize * 4U;
          pCleanupMainBar->pMappedMem    = pBorderPixels;
          pCleanupMainBar->fd            = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Vertical Border
      for (std::uint8_t i{ 2U }; i < Patache::sBorderVerticalCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              borderFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, Patache::sBorderThresholdEdgeCSDSize
                                               * (rHeight + Patache::sMainbarHeightCSDSize) * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    Patache::sBorderThresholdEdgeCSDSize
                        * (rHeight + Patache::sMainbarHeightCSDSize) * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U }; i2 < Patache::sBorderThresholdEdgeCSDSize
                                                * (rHeight + Patache::sMainbarHeightCSDSize);
               ++i2)
            {
              pBorderPixels[i2] = Patache::borderFocusCSDColor;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              Patache::sBorderThresholdEdgeCSDSize * (rHeight + Patache::sMainbarHeightCSDSize)
                  * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sBorderThresholdEdgeCSDSize,
              (rHeight + Patache::sMainbarHeightCSDSize), Patache::sBorderThresholdEdgeCSDSize * 4U,
              WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    Patache::sBorderThresholdEdgeCSDSize,
                                    (rHeight + Patache::sMainbarHeightCSDSize));

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize = Patache::sBorderThresholdEdgeCSDSize
                                           * (rHeight + Patache::sMainbarHeightCSDSize) * 4U;
          pCleanupMainBar->pMappedMem    = pBorderPixels;
          pCleanupMainBar->fd            = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Corner Border
      for (std::uint8_t i{ 4U }; i < Patache::sBorderCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              borderFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t borderFileDescriptor{ shm_open (borderFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (borderFileDescriptorName[i]);

          ftruncate (borderFileDescriptor, Patache::sBorderThresholdEdgeCSDSize
                                               * Patache::sBorderThresholdEdgeCSDSize * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (mmap (
              nullptr,
              Patache::sBorderThresholdEdgeCSDSize * Patache::sBorderThresholdEdgeCSDSize * 4U,
              PROT_READ | PROT_WRITE, MAP_SHARED, borderFileDescriptor, 0)) };

          for (std::uint32_t i2{ 0U };
               i2 < (Patache::sBorderThresholdEdgeCSDSize * Patache::sBorderThresholdEdgeCSDSize);
               ++i2)
            {
              pBorderPixels[i2] = Patache::borderFocusCSDColor;
            }

          wl_shm_pool * pPool{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, borderFileDescriptor,
              Patache::sBorderThresholdEdgeCSDSize * Patache::sBorderThresholdEdgeCSDSize * 4U) };

          wl_buffer * pBuffer{ wl_shm_pool_create_buffer (
              pPool, 0, Patache::sBorderThresholdEdgeCSDSize, Patache::sBorderThresholdEdgeCSDSize,
              Patache::sBorderThresholdEdgeCSDSize * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPool);

          wl_surface_attach (pEngine->waylandWindow.pBorderSurface[i], pBuffer, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pBorderSurface[i], 0, 0,
                                    Patache::sBorderThresholdEdgeCSDSize,
                                    Patache::sBorderThresholdEdgeCSDSize);

          Patache::SurfaceBufferCleanup * pCleanup{ static_cast<Patache::SurfaceBufferCleanup *> (
              std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup))) };

          pCleanupMainBar->mappedMemSize
              = Patache::sBorderThresholdEdgeCSDSize * Patache::sBorderThresholdEdgeCSDSize * 4U;
          pCleanupMainBar->pMappedMem = pBorderPixels;
          pCleanupMainBar->fd         = borderFileDescriptor;

          wl_buffer_add_listener (pBuffer, &wl_buffer_listener, pCleanup);

          wl_surface_commit (pEngine->waylandWindow.pBorderSurface[i]);
        }

      // Shadow
      for (std::uint8_t i{ 0U }; i < Patache::sBorderCSDSize; ++i)
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
          -Patache::sBorderThresholdEdgeCSDSize, Patache::shadowHorizontalPositionY);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottom],
          -Patache::sBorderThresholdEdgeCSDSize,
          (rHeight + Patache::sBorderThresholdEdgeCSDSize + Patache::sBorderThresholdEdgeCSDSize));

      // Vertical Shadow
      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eLeft],
          Patache::shadowVerticalPositionX, Patache::shadowVerticalPositionY);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eRight],
          (rWidth + Patache::sBorderThresholdEdgeCSDSize), Patache::shadowVerticalPositionY);

      // Corner Shadow
      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopLeft],
          Patache::shadowVerticalPositionX, Patache::shadowHorizontalPositionY);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eTopRight],
          (rWidth + Patache::sBorderThresholdEdgeCSDSize), Patache::shadowHorizontalPositionY);

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomLeft],
          Patache::shadowVerticalPositionX,
          (rHeight + Patache::sBorderThresholdEdgeCSDSize + Patache::sShadowThresholdEdgeCSDSize));

      wl_subsurface_set_position (
          pEngine->waylandWindow.pShadowSubSurface[Patache::BorderIndexCSD::eBottomRight],
          (rWidth + Patache::sBorderThresholdEdgeCSDSize),
          (rHeight + Patache::sBorderThresholdEdgeCSDSize + Patache::sShadowThresholdEdgeCSDSize));

      // Horizontal
      for (std::uint8_t i{ 0U }; i < Patache::sBorderHorizontalCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              shadowFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                               * Patache::sShadowThresholdEdgeCSDSize * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                        * Patache::sShadowThresholdEdgeCSDSize * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eTop)
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                            * Patache::sShadowThresholdEdgeCSDSize;
                   ++i2)
                {
                  if ((i2 % (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                           * Patache::sShadowThresholdEdgeCSDSize
                       == 0)
                      && i3 < Patache::sShadowTopCSDSize)
                    {
                      ++i3;
                    }

                  pBorderPixels[i2] = sShadowTop[i3];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                            * Patache::sShadowThresholdEdgeCSDSize;
                   ++i2)
                {
                  if ((i2 % (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
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
              (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                  * Patache::sShadowThresholdEdgeCSDSize * 4U) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2)),
              Patache::sShadowThresholdEdgeCSDSize,
              (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2)) * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2)),
                                    Patache::sShadowThresholdEdgeCSDSize);

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = (rWidth + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                          * Patache::sShadowThresholdEdgeCSDSize * 4U;
          pCleanupShadow->pMappedMem    = pBorderPixels;
          pCleanupShadow->fd            = shadowFileDescriptor;

          wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

          wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
        }

      // Vertical
      for (std::uint8_t i{ Patache::sBorderHorizontalCSDSize }; i < Patache::sBorderVerticalCSDSize;
           ++i)
        {
          Patache::RandomizeFileDescriptorName (
              shadowFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, Patache::sShadowThresholdEdgeCSDSize
                                               * (rHeight + Patache::sMainbarHeightCSDSize
                                                  + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                               * 4U);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr,
                    Patache::sShadowThresholdEdgeCSDSize
                        * (rHeight + Patache::sMainbarHeightCSDSize
                           + (Patache::sBorderThresholdEdgeCSDSize * 2))
                        * 4U,
                    PROT_READ | PROT_WRITE, MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eRight)
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (Patache::sShadowThresholdEdgeCSDSize
                         * (rHeight + Patache::sMainbarHeightCSDSize
                            + (Patache::sBorderThresholdEdgeCSDSize * 2)));
                   ++i2, ++i3)
                {
                  if ((i2 % Patache::sShadowThresholdEdgeCSDSize == 0)
                      || i3 < Patache::sShadowRightCSDSize)
                    {
                      i3 = 0;
                    }

                  pBorderPixels[i2] = sShadowRight[i3];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }, i3{ 0U };
                   i2 < (Patache::sShadowThresholdEdgeCSDSize
                         * (rHeight + Patache::sMainbarHeightCSDSize
                            + (Patache::sBorderThresholdEdgeCSDSize * 2)));
                   ++i2, ++i3)
                {
                  if ((i2 % Patache::sShadowThresholdEdgeCSDSize == 0)
                      || i3 < Patache::sShadowLeftCSDSize)
                    {
                      i3 = 0;
                    }

                  pBorderPixels[i2] = Patache::sShadowLeftCSD[i3];
                }
            }

          wl_shm_pool * pPoolTest{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
              Patache::sShadowThresholdEdgeCSDSize
                  * (rHeight + Patache::sMainbarHeightCSDSize
                     + (Patache::sBorderThresholdEdgeCSDSize * 2))
                  * 4U) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, Patache::sShadowThresholdEdgeCSDSize,
              (rHeight + Patache::sMainbarHeightCSDSize
               + (Patache::sBorderThresholdEdgeCSDSize * 2)),
              Patache::sShadowThresholdEdgeCSDSize * 4U, WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    Patache::sShadowThresholdEdgeCSDSize,
                                    (rHeight + Patache::sMainbarHeightCSDSize
                                     + (Patache::sBorderThresholdEdgeCSDSize * 2)));

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = Patache::sShadowThresholdEdgeCSDSize
                                          * (rHeight + Patache::sMainbarHeightCSDSize
                                             + (Patache::sBorderThresholdEdgeCSDSize * 2))
                                          * 4U;
          pCleanupShadow->pMappedMem    = pBorderPixels;
          pCleanupShadow->fd            = shadowFileDescriptor;

          wl_buffer_add_listener (pBufferTest, &wl_buffer_listener, pCleanupShadow);

          wl_surface_commit (pEngine->waylandWindow.pShadowSurface[i]);
        }

      // Corner
      for (std::uint8_t i{ Patache::sBorderVerticalCSDSize }; i < Patache::sBorderCSDSize; ++i)
        {
          Patache::RandomizeFileDescriptorName (
              shadowFileDescriptorName[i], Patache::randomizeFdStrBuffSize, fdDist, fdGenerator);

          std::int32_t shadowFileDescriptor{ shm_open (shadowFileDescriptorName[i],
                                                       O_RDWR | O_CREAT | O_EXCL,
                                                       S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH) };

          shm_unlink (shadowFileDescriptorName[i]);

          ftruncate (shadowFileDescriptor, Patache::shadowCornerCSDTotalByteSize);

          std::uint32_t * pBorderPixels{ static_cast<std::uint32_t *> (
              mmap (nullptr, Patache::shadowCornerCSDTotalByteSize, PROT_READ | PROT_WRITE,
                    MAP_SHARED, shadowFileDescriptor, 0)) };

          if (i == Patache::BorderIndexCSD::eTopLeft)
            {
              for (std::uint32_t i2{ 0U }; i2 < Patache::shadowCornerCSDSize; ++i2)
                {
                  pBorderPixels[i2] = sShadowTopLeft[i2];
                }
            }
          else if (i == Patache::BorderIndexCSD::eTopRight)
            {
              for (std::uint32_t i2{ 0U }; i2 < Patache::shadowCornerCSDSize; ++i2)
                {
                  pBorderPixels[i2] = sShadowTopRight[i2];
                }
            }
          else if (i == Patache::BorderIndexCSD::eBottomRight)
            {
              for (std::uint32_t i2{ 0U }; i2 < Patache::shadowCornerCSDSize; ++i2)
                {
                  pBorderPixels[i2] = sShadowBottomRight[i2];
                }
            }
          else
            {
              for (std::uint32_t i2{ 0U }; i2 < Patache::shadowCornerCSDSize; ++i2)
                {
                  pBorderPixels[i2] = sShadowBottomLeft[i2];
                }
            }

          wl_shm_pool * pPoolTest{ wl_shm_create_pool (
              pEngine->waylandWindow.pDecorationSharedMemory, shadowFileDescriptor,
              Patache::shadowCornerCSDTotalByteSize) };

          wl_buffer * pBufferTest{ wl_shm_pool_create_buffer (
              pPoolTest, 0, Patache::sShadowThresholdEdgeCSDSize,
              Patache::sShadowThresholdEdgeCSDSize, Patache::shadowCornerWidthCSDByteSize,
              WL_SHM_FORMAT_ARGB8888) };

          wl_shm_pool_destroy (pPoolTest);

          wl_surface_attach (pEngine->waylandWindow.pShadowSurface[i], pBufferTest, 0, 0);

          wl_surface_damage_buffer (pEngine->waylandWindow.pShadowSurface[i], 0, 0,
                                    Patache::sShadowThresholdEdgeCSDSize,
                                    Patache::sShadowThresholdEdgeCSDSize);

          Patache::SurfaceBufferCleanup * pCleanupShadow{
            static_cast<Patache::SurfaceBufferCleanup *> (
                std::calloc (1ZU, sizeof (Patache::SurfaceBufferCleanup)))
          };

          pCleanupShadow->mappedMemSize = Patache::shadowCornerCSDTotalByteSize;
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
                          SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, false);

  SDL_SetBooleanProperty (windowProperties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN,
                          true);

  pEngine->pGameWindow = SDL_CreateWindowWithProperties (windowProperties);

  if (pEngine->pGameWindow == nullptr)
    {
      Patache::FatalErrorMessage ("Patache Engine - SDL & Wayland",
                                  "SDL Cannot import external surfaces. if you are "
                                  "using Gamescope try with --expose-wayland",
                                  pEngine->configuration);

      return false;
    }

  return true;
}
