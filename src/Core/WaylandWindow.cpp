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

  wl_registry_add_listener (Registry, &RegistryListener, Engine);

  wl_display_roundtrip (Engine->WaylandWindow.Display);

  wl_registry_destroy (Registry);
  Registry = nullptr;

  // Errors
  if (Engine->WaylandWindow.Compositor == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Wayland",
                        "Missing wl_compositor in this compositor",
                        std::cref (Engine->configuration));

      return false;
    }
  else if (Engine->WaylandWindow.WindowManangerBase == nullptr)
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Wayland", "Missing xdg_wm_base in this compositor",
          std::cref (Engine->configuration));

      return false;
    }

  // Create Wayland Surface
  Engine->WaylandWindow.Surface
      = wl_compositor_create_surface (Engine->WaylandWindow.Compositor);

  // Get Shell Surface
  Engine->WaylandWindow.DesktopStyleUserInterface
      = xdg_wm_base_get_xdg_surface (Engine->WaylandWindow.WindowManangerBase,
                                     Engine->WaylandWindow.Surface);

  xdg_surface_add_listener (Engine->WaylandWindow.DesktopStyleUserInterface,
                            &DesktopStyleUserInterfaceListener, Engine);

  // Get xdg_toplevel
  Engine->WaylandWindow.DesktopWindow = xdg_surface_get_toplevel (
      Engine->WaylandWindow.DesktopStyleUserInterface);

  Engine->Vulkan.SwapChainExtent.width  = width;
  Engine->Vulkan.SwapChainExtent.height = height;

  xdg_toplevel_set_min_size (Engine->WaylandWindow.DesktopWindow, 640, 360);

  // Callback for Window events form the desktop
  xdg_toplevel_add_listener (Engine->WaylandWindow.DesktopWindow,
                             &DesktopWindowListener, Engine);

  xdg_toplevel_set_title (Engine->WaylandWindow.DesktopWindow, WindowTitle);
  xdg_toplevel_set_app_id (Engine->WaylandWindow.DesktopWindow, WindowTitle);

  wl_surface_commit (Engine->WaylandWindow.Surface);
  wl_display_roundtrip (Engine->WaylandWindow.Display);
  wl_surface_commit (Engine->WaylandWindow.Surface);

  // If Server Side Decorations is no available
  if (Engine->WaylandWindow.DecorationMananger == nullptr)
    {
      if (Engine->WaylandWindow.SubCompositor == nullptr)
        {
          std::future<void> Err
              = std::async (std::launch::async, Patache::Log::WarningMessage,
                            "Missing wl_subcompositor in this compositor. "
                            "continuing without CSD");

          goto SKIP_CSD_CREATION;
        }
      else if (Engine->WaylandWindow.DecorationSharedMemory == nullptr)
        {
          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::FatalErrorMessage,
              "Patache Engine - Wayland", "Missing wl_shm in this compositor",
              std::cref (Engine->configuration));

          return false;
        }
      else if (Engine->WaylandWindow.Input == nullptr)
        {
          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::FatalErrorMessage,
              "Patache Engine - Wayland", "Missing wl_seat in this compositor",
              std::cref (Engine->configuration));

          return false;
        }

      // Decoration
      /*
       * Main Bar
       * [||||||||||||||||||||||]
       */
      Engine->WaylandWindow.MainBarSurface
          = wl_compositor_create_surface (Engine->WaylandWindow.Compositor);

      Engine->WaylandWindow.MainBarSubSurface
          = wl_subcompositor_get_subsurface (
              Engine->WaylandWindow.SubCompositor,
              Engine->WaylandWindow
                  .MainBarSurface, // surface turned into a sub-surface
              Engine->WaylandWindow.Surface); // the parent surface

      wl_subsurface_set_position (Engine->WaylandWindow.MainBarSubSurface, 0,
                                  -PATACHE_CSD_HEIGHT_SIZE);

      // Create a file descriptor for a buffer
      char         MainBarName[] = "Patache-MainBar";
      std::int32_t MainBarFileDescriptor
          = shm_open (MainBarName, O_RDWR | O_CREAT | O_EXCL,
                      S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
      shm_unlink (MainBarName);
      ftruncate (MainBarFileDescriptor, Engine->Vulkan.SwapChainExtent.width
                                            * PATACHE_CSD_HEIGHT_SIZE * 4);

      /*
       * Map File to a memory (To write the color) to be used for a
       * wl_buffer
       */
      Engine->WaylandWindow.MainBarPixels = static_cast<std::uint32_t *> (
          mmap (nullptr,
                Engine->Vulkan.SwapChainExtent.width * PATACHE_CSD_HEIGHT_SIZE
                    * 4,
                PROT_READ | PROT_WRITE, MAP_SHARED, MainBarFileDescriptor, 0));

      for (std::uint32_t i = 0; i < (Engine->Vulkan.SwapChainExtent.width
                                     * PATACHE_CSD_HEIGHT_SIZE);
           ++i)
        {
          // Write Color for a wl_buffer
          Engine->WaylandWindow.MainBarPixels[i] = PATACHE_CSD_FOCUS_COLOR;
        }

      wl_shm_pool * DecorationPool = wl_shm_create_pool (
          Engine->WaylandWindow.DecorationSharedMemory, MainBarFileDescriptor,
          Engine->Vulkan.SwapChainExtent.width * PATACHE_CSD_HEIGHT_SIZE * 4);

      Engine->WaylandWindow.MainBarBuffer = wl_shm_pool_create_buffer (
          DecorationPool, 0, Engine->Vulkan.SwapChainExtent.width,
          PATACHE_CSD_HEIGHT_SIZE, Engine->Vulkan.SwapChainExtent.width * 4,
          WL_SHM_FORMAT_ARGB8888);

      wl_surface_attach (Engine->WaylandWindow.MainBarSurface,
                         Engine->WaylandWindow.MainBarBuffer, 0, 0);
      wl_surface_damage_buffer (Engine->WaylandWindow.MainBarSurface, 0, 0,
                                Engine->Vulkan.SwapChainExtent.width,
                                PATACHE_CSD_HEIGHT_SIZE);

      wl_shm_pool_destroy (DecorationPool);
      close (MainBarFileDescriptor);

      wl_surface_commit (Engine->WaylandWindow.MainBarSurface);

      /*
       * Buttons
       * [MINIMIZE | MAXIMIZE | CLOSE]
       */
      for (std::uint8_t i = 0; i < 3; ++i)
        {
          Engine->WaylandWindow.ButtonSurface[i]
              = wl_compositor_create_surface (
                  Engine->WaylandWindow.Compositor);

          Engine->WaylandWindow.ButtonSubSurface[i]
              = wl_subcompositor_get_subsurface (
                  Engine->WaylandWindow.SubCompositor,
                  Engine->WaylandWindow.ButtonSurface[i], // surface turned
                                                          // into a sub-surface
                  Engine->WaylandWindow.MainBarSurface);  // the parent surface
        }

      // Set button position
      wl_subsurface_set_position (
          Engine->WaylandWindow
              .ButtonSubSurface[PATACHE_CSD_MINIMIZE_BUTTON_INDEX],
          (width - 89), 2);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .ButtonSubSurface[PATACHE_CSD_MAXIMIZE_BUTTON_INDEX],
          (width - 59), 2);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .ButtonSubSurface[PATACHE_CSD_CLOSE_BUTTON_INDEX],
          (width - 29), 2);

      for (std::uint8_t i = 0; i < 3; ++i)
        {
          // Create a file descriptor for a buffer
          char         CSDButtonFileName[] = "Patache-Button-File";
          std::int32_t CSDButtonFileDescriptor
              = shm_open (CSDButtonFileName, O_RDWR | O_CREAT | O_EXCL,
                          S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
          shm_unlink (CSDButtonFileName);
          ftruncate (CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow.DecorationButtonPixels[i]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr, PATACHE_CSD_CLOSE_BUTTON_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDButtonFileDescriptor, 0));

          switch (i)
            {
            case PATACHE_CSD_MINIMIZE_BUTTON_INDEX:
              for (std::uint32_t i2 = 0;
                   i2 < PATACHE_CSD_MINIMIZE_BUTTON_PIXELCOUNT; ++i2)
                {
                  Engine->WaylandWindow.DecorationButtonPixels
                      [PATACHE_CSD_MINIMIZE_BUTTON_INDEX][i2]
                      = CSDMinimizeButton[i2];
                }
              break;

            case PATACHE_CSD_MAXIMIZE_BUTTON_INDEX:
              for (std::uint32_t i2 = 0;
                   i2 < PATACHE_CSD_MAXIMIZE_BUTTON_PIXELCOUNT; ++i2)
                {
                  Engine->WaylandWindow.DecorationButtonPixels
                      [PATACHE_CSD_MAXIMIZE_BUTTON_INDEX][i2]
                      = CSDMaximizeButton[i2];
                }
              break;

            case PATACHE_CSD_CLOSE_BUTTON_INDEX:
              for (std::uint32_t i2 = 0;
                   i2 < PATACHE_CSD_CLOSE_BUTTON_PIXELCOUNT; ++i2)
                {
                  Engine->WaylandWindow
                      .DecorationButtonPixels[PATACHE_CSD_CLOSE_BUTTON_INDEX]
                                             [i2]
                      = CSDCloseButton[i2];
                }
              break;
            }

          wl_shm_pool * DecorationButtonPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDButtonFileDescriptor, PATACHE_CSD_CLOSE_BUTTON_SIZE);

          Engine->WaylandWindow.DecorationButtonBuffer[i]
              = wl_shm_pool_create_buffer (
                  DecorationButtonPool, 0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                  PATACHE_CSD_CLOSE_BUTTON_HEIGHT,
                  PATACHE_CSD_CLOSE_BUTTON_WIDTH * 4, WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (Engine->WaylandWindow.ButtonSurface[i],
                             Engine->WaylandWindow.DecorationButtonBuffer[i],
                             0, 0);

          wl_surface_damage_buffer (Engine->WaylandWindow.ButtonSurface[i], 0,
                                    0, PATACHE_CSD_CLOSE_BUTTON_WIDTH,
                                    PATACHE_CSD_CLOSE_BUTTON_HEIGHT);

          wl_shm_pool_destroy (DecorationButtonPool);
          close (CSDButtonFileDescriptor);

          wl_surface_commit (Engine->WaylandWindow.ButtonSurface[i]);
        }

      // Border Window
      for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_SIZE; ++i)
        {
          Engine->WaylandWindow.BorderSurface[i]
              = wl_compositor_create_surface (
                  Engine->WaylandWindow.Compositor);

          Engine->WaylandWindow.BorderSubSurface[i]
              = wl_subcompositor_get_subsurface (
                  Engine->WaylandWindow.SubCompositor,
                  Engine->WaylandWindow.BorderSurface[i], // surface turned
                                                          // into a sub-surface
                  Engine->WaylandWindow.Surface);         // the parent surface
        }

      // Horizontal border
      wl_subsurface_set_position (
          Engine->WaylandWindow.BorderSubSurface[PATACHE_CSD_TOP_BORDER_INDEX],
          0, -26);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_BOTTOM_BORDER_INDEX],
          0, height);

      // Vertical Border
      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_LEFT_BORDER_INDEX],
          -PATACHE_CSD_THRESHOLD_EDGE_SIZE, -PATACHE_CSD_HEIGHT_SIZE);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_RIGHT_BORDER_INDEX],
          width, -PATACHE_CSD_HEIGHT_SIZE);

      // Corner Borders
      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_TOPLEFT_BORDER_INDEX],
          -PATACHE_CSD_THRESHOLD_EDGE_SIZE, -26);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_TOPRIGHT_BORDER_INDEX],
          width, -26);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_BOTTOMLEFT_BORDER_INDEX],
          -PATACHE_CSD_THRESHOLD_EDGE_SIZE, height);

      wl_subsurface_set_position (
          Engine->WaylandWindow
              .BorderSubSurface[PATACHE_CSD_BOTTOMRIGHT_BORDER_INDEX],
          width, height);

      // Horizontal border
      for (std::uint8_t i = 0; i < PATACHE_CSD_BORDER_HORIZONTAL_SIZE; ++i)
        {
          // Create a file descriptor for a buffer
          char         CSDBorderFileName[] = "Patache-Border";
          std::int32_t CSDBorderFileDescriptor
              = shm_open (CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL,
                          S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
          shm_unlink (CSDBorderFileName);
          ftruncate (CSDBorderFileDescriptor,
                     width * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow.BorderPixels[i]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr, width * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDBorderFileDescriptor, 0));

          for (std::uint32_t i2 = 0;
               i2 < (width * PATACHE_CSD_THRESHOLD_EDGE_SIZE); ++i2)
            {
              Engine->WaylandWindow.BorderPixels[i][i2]
                  = PATACHE_CSD_BORDER_COLOR;
            }

          wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDBorderFileDescriptor,
              width * PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4);

          Engine->WaylandWindow.BorderBuffer[i] = wl_shm_pool_create_buffer (
              DecorationBorderPool, 0, width, PATACHE_CSD_THRESHOLD_EDGE_SIZE,
              width * 4, WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                             Engine->WaylandWindow.BorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (Engine->WaylandWindow.BorderSurface[i], 0,
                                    0, width, PATACHE_CSD_THRESHOLD_EDGE_SIZE);

          wl_shm_pool_destroy (DecorationBorderPool);
          close (CSDBorderFileDescriptor);

          wl_surface_commit (Engine->WaylandWindow.BorderSurface[i]);
        }

      // Vertical Border
      for (std::uint8_t i = 2; i < PATACHE_CSD_BORDER_VERTICAL_SIZE; ++i)
        {
          // Create a file descriptor for a buffer
          char         CSDBorderFileName[] = "Patache-Border";
          std::int32_t CSDBorderFileDescriptor
              = shm_open (CSDBorderFileName, O_RDWR | O_CREAT | O_EXCL,
                          S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
          shm_unlink (CSDBorderFileName);
          ftruncate (CSDBorderFileDescriptor,
                     PATACHE_CSD_THRESHOLD_EDGE_SIZE
                         * (height + PATACHE_CSD_HEIGHT_SIZE) * 4);

          /*
           * Map File to a memory (To write the color) to be used for a
           * wl_buffer
           */
          Engine->WaylandWindow.BorderPixels[i]
              = static_cast<std::uint32_t *> (
                  mmap (nullptr,
                        PATACHE_CSD_THRESHOLD_EDGE_SIZE
                            * (height + PATACHE_CSD_HEIGHT_SIZE) * 4,
                        PROT_READ | PROT_WRITE, MAP_SHARED,
                        CSDBorderFileDescriptor, 0));

          for (std::uint32_t i2 = 0;
               i2 < PATACHE_CSD_THRESHOLD_EDGE_SIZE
                        * (height + PATACHE_CSD_HEIGHT_SIZE);
               ++i2)
            {
              Engine->WaylandWindow.BorderPixels[i][i2]
                  = PATACHE_CSD_BORDER_COLOR;
            }

          wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDBorderFileDescriptor,
              PATACHE_CSD_THRESHOLD_EDGE_SIZE
                  * (height + PATACHE_CSD_HEIGHT_SIZE) * 4);

          Engine->WaylandWindow.BorderBuffer[i] = wl_shm_pool_create_buffer (
              DecorationBorderPool, 0, PATACHE_CSD_THRESHOLD_EDGE_SIZE,
              (height + PATACHE_CSD_HEIGHT_SIZE),
              PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4, WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                             Engine->WaylandWindow.BorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (Engine->WaylandWindow.BorderSurface[i], 0,
                                    0, PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                                    (height + PATACHE_CSD_HEIGHT_SIZE));

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

          for (std::uint32_t i2 = 0; i2 < (PATACHE_CSD_THRESHOLD_EDGE_SIZE
                                           * PATACHE_CSD_THRESHOLD_EDGE_SIZE);
               ++i2)
            {
              Engine->WaylandWindow.BorderPixels[i][i2]
                  = PATACHE_CSD_BORDER_COLOR;
            }

          wl_shm_pool * DecorationBorderPool = wl_shm_create_pool (
              Engine->WaylandWindow.DecorationSharedMemory,
              CSDBorderFileDescriptor,
              PATACHE_CSD_THRESHOLD_EDGE_SIZE * PATACHE_CSD_THRESHOLD_EDGE_SIZE
                  * 4);

          Engine->WaylandWindow.BorderBuffer[i] = wl_shm_pool_create_buffer (
              DecorationBorderPool, 0, PATACHE_CSD_THRESHOLD_EDGE_SIZE,
              PATACHE_CSD_THRESHOLD_EDGE_SIZE,
              PATACHE_CSD_THRESHOLD_EDGE_SIZE * 4, WL_SHM_FORMAT_ARGB8888);

          wl_surface_attach (Engine->WaylandWindow.BorderSurface[i],
                             Engine->WaylandWindow.BorderBuffer[i], 0, 0);

          wl_surface_damage_buffer (Engine->WaylandWindow.BorderSurface[i], 0,
                                    0, PATACHE_CSD_THRESHOLD_EDGE_SIZE,
                                    PATACHE_CSD_THRESHOLD_EDGE_SIZE);

          wl_shm_pool_destroy (DecorationBorderPool);
          close (CSDBorderFileDescriptor);

          wl_surface_commit (Engine->WaylandWindow.BorderSurface[i]);
        }
    }
  else
    {
      // Has server side decoration
      Engine->WaylandWindow.Decoration
          = zxdg_decoration_manager_v1_get_toplevel_decoration (
              Engine->WaylandWindow.DecorationMananger,
              Engine->WaylandWindow.DesktopWindow);

      zxdg_toplevel_decoration_v1_set_mode (
          Engine->WaylandWindow.Decoration,
          ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    }

SKIP_CSD_CREATION:

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
      SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, false);

  SDL_SetBooleanProperty (WindowProperties,
                          SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN,
                          true);

  Engine->GameWindow = SDL_CreateWindowWithProperties (WindowProperties);

  if (Engine->GameWindow == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - SDL & Wayland",
                        "SDL Cannot import external surfaces. if you are "
                        "using Gamescope try with --expose-wayland",
                        std::cref (Engine->configuration));

      return false;
    }

  return true;
}
