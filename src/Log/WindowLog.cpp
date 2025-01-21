#include "WindowLog.hpp"

void
Patata::Engine::WindowLog (void)
{
  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (GameWindow).name (),
                                             nullptr, nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (GameWindow).name () }, "] ",
#endif
      PATATA_TERM_BOLD,
#endif // PATATA_DEBUG
      PATATA_TERM_COLOR_PATATA, "Window ", PATATA_TERM_RESET, "INFO");

  // Desktop
  const char * XDG_CURRENT_DESKTOP = getenv ("XDG_CURRENT_DESKTOP");

  // engineInfo / Desktop Type
#if PATATA_DEBUG == 1
  if (strcmp (XDG_CURRENT_DESKTOP, "GNOME") == 0
      || strcmp (XDG_CURRENT_DESKTOP, "GNOME-Classic") == 0)
    engineInfo.DesktopType = Patata::Desktop::Gnome;

  if (strcmp (XDG_CURRENT_DESKTOP, "KDE") == 0)
    engineInfo.DesktopType = Patata::Desktop::KdePlasma;

  if (strcmp (XDG_CURRENT_DESKTOP, "XFCE") == 0)
    engineInfo.DesktopType = Patata::Desktop::Xfce;

  if (strcmp (XDG_CURRENT_DESKTOP, "Pantheon") == 0)
    engineInfo.DesktopType = Patata::Desktop::Pantheon;

  if (strcmp (XDG_CURRENT_DESKTOP, "MATE") == 0)
    engineInfo.DesktopType = Patata::Desktop::Mate;

  if (strcmp (XDG_CURRENT_DESKTOP, "Cinnamon") == 0)
    engineInfo.DesktopType = Patata::Desktop::Cinnamon;

  if (strcmp (XDG_CURRENT_DESKTOP, "LXQt") == 0)
    engineInfo.DesktopType = Patata::Desktop::Lxqt;

  if (strcmp (XDG_CURRENT_DESKTOP, "Unity") == 0)
    engineInfo.DesktopType = Patata::Desktop::Unity;

  if (strcmp (XDG_CURRENT_DESKTOP, "COSMIC") == 0)
    engineInfo.DesktopType = Patata::Desktop::Cosmic;
#endif

  if (XDG_CURRENT_DESKTOP != nullptr)
    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "  [",
        std::string_view{ abi::__cxa_demangle (
            typeid (XDG_CURRENT_DESKTOP).name (), nullptr, nullptr, nullptr) },
        "] ",
#else
        "  [", std::string_view{ typeid (XDG_CURRENT_DESKTOP).name () }, "] ",
#endif
        PATATA_TERM_RESET,
#else  // PATATA_DEBUG
        "  ",
#endif // PATATA_DEBUG
        PATATA_TERM_BOLD, "Desktop : ", PATATA_TERM_RESET,
        std::string_view{ XDG_CURRENT_DESKTOP });

  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (GameWindow, &WindowInfo);
  const char * XDG_SESSION_TYPE = getenv ("XDG_SESSION_TYPE");

  switch (WindowInfo.subsystem)
    {
    case SDL_SYSWM_WAYLAND:
      fast_io::io::println (
#if PATATA_DEBUG == 1
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "  [",
          std::string_view{
              abi::__cxa_demangle (typeid (WindowInfo.info.wl.display).name (),
                                   nullptr, nullptr, nullptr) },
          "] ",
#else
          "  [",
          std::string_view{ typeid (WindowInfo.info.wl.display).name () },
          "] ",
#endif
          PATATA_TERM_RESET,
#else  // PATATA_DEBUG
          "  ",
#endif // PATATA_DEBUG
          PATATA_TERM_BOLD, "Window System : ", PATATA_TERM_RESET, "Wayland ");

      // Wayland Display
      if (WindowInfo.info.wl.display != nullptr)
        fast_io::io::println (
#if PATATA_DEBUG == 1
            PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            "  [",
            std::string_view{ abi::__cxa_demangle (
                typeid (WindowInfo.info.wl.display).name (), nullptr, nullptr,
                nullptr) },
            "] ",
#else
            "  [",
            std::string_view{ typeid (WindowInfo.info.wl.display).name () },
            "] ",
#endif
            PATATA_TERM_RESET,
#else  // PATATA_DEBUG
            "  ",
#endif // PATATA_DEBUG
            PATATA_TERM_BOLD, "Wayland display", PATATA_TERM_RESET);

      // Wayland surface
      if (WindowInfo.info.wl.surface != nullptr)
        fast_io::io::println (
#if PATATA_DEBUG == 1
            PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            "  [",
            std::string_view{ abi::__cxa_demangle (
                typeid (WindowInfo.info.wl.surface).name (), nullptr, nullptr,
                nullptr) },
            "] ",
#else
            "  [",
            std::string_view{ typeid (WindowInfo.info.wl.surface).name () },
            "] ",
#endif
            PATATA_TERM_RESET,
#else  // PATATA_DEBUG
            "  ",
#endif // PATATA_DEBUG
            PATATA_TERM_BOLD, "Wayland surface", PATATA_TERM_RESET);

      // Wayland xdg surface (window manager handle)
      if (WindowInfo.info.wl.xdg_surface != nullptr)
        fast_io::io::println (
#if PATATA_DEBUG == 1
            PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            "  [",
            std::string_view{ abi::__cxa_demangle (
                typeid (WindowInfo.info.wl.xdg_surface).name (), nullptr,
                nullptr, nullptr) },
            "] ",
#else
            "  [",
            std::string_view{
                typeid (WindowInfo.info.wl.xdg_surface).name () },
            "] ",
#endif
            PATATA_TERM_RESET,
#else  // PATATA_DEBUG
            "  ",
#endif // PATATA_DEBUG
            PATATA_TERM_BOLD, "Wayland xdg surface (window manager handle)",
            PATATA_TERM_RESET);

      // Wayland EGL window (native window)
      if (WindowInfo.info.wl.egl_window != nullptr)
        fast_io::io::println (
#if PATATA_DEBUG == 1
            PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            "  [",
            std::string_view{ abi::__cxa_demangle (
                typeid (WindowInfo.info.wl.egl_window).name (), nullptr,
                nullptr, nullptr) },
            "] ",
#else
            "  [",
            std::string_view{ typeid (WindowInfo.info.wl.egl_window).name () },
            "] ",
#endif
            PATATA_TERM_RESET,
#else  // PATATA_DEBUG
            "  ",
#endif // PATATA_DEBUG
            PATATA_TERM_BOLD, "Wayland EGL window (native window)",
            PATATA_TERM_RESET);

#if PATATA_DEBUG == 1
      engineInfo.WindowType = Patata::WindowType::Wayland;
#endif
      break;

#if defined(PATATA_LINUX_XORG_SUPPORT) && defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
      if (strcmp (XDG_SESSION_TYPE, "wayland") == 0)
        fast_io::io::println (PATATA_TERM_BOLD,
                              "  Window System : ", PATATA_TERM_RESET,
                              "XWayland",
#if PATATA_DEBUG == 1
                              PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                              " [(",
                              std::string_view{ abi::__cxa_demangle (
                                  typeid (XDG_SESSION_TYPE).name (), nullptr,
                                  nullptr, nullptr) },
                              ") XDG_SESSION_TYPE]",
#else
                              " [(",
                              std::string_view{
                                  typeid (XDG_SESSION_TYPE).name () },
                              ") XDG_SESSION_TYPE]",
#endif
                              PATATA_TERM_RESET,
#endif // PATATA_DEBUG
                              " | ", std::string_view{ XDG_SESSION_TYPE });
      else
        {
          fast_io::io::println (PATATA_TERM_BOLD,
                                "  Window System : ", PATATA_TERM_RESET, "X",
#if PATATA_DEBUG == 1
                                PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                                " [(",
                                std::string_view{ abi::__cxa_demangle (
                                    typeid (XDG_SESSION_TYPE).name (), nullptr,
                                    nullptr, nullptr) },
                                ") XDG_SESSION_TYPE]",
#else
                                " [(",
                                std::string_view{
                                    typeid (XDG_SESSION_TYPE).name () },
                                ") XDG_SESSION_TYPE]",
#endif
                                PATATA_TERM_RESET,
#endif // PATATA_DEBUG
                                " | ", std::string_view{ XDG_SESSION_TYPE });

          if (!WindowInfo.info.x11.window)
            fast_io::io::println (
#if PATATA_DEBUG == 1
                PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                "  [",
                std::string_view{ abi::__cxa_demangle (
                    typeid (WindowInfo.info.x11.window).name (), nullptr,
                    nullptr, nullptr) },
                "]",
#else
                "  [",
                std::string_view{
                    typeid (WindowInfo.info.x11.window).name () },
                "]",
#endif
                PATATA_TERM_RESET,
#else  // PATATA_DEBUG
                "  ",
#endif // PATATA_DEBUG
                PATATA_TERM_BOLD, "The X11 window", PATATA_TERM_RESET);

          if (WindowInfo.info.x11.display != nullptr)
            fast_io::io::println (
#if PATATA_DEBUG == 1
                PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                "  [",
                std::string_view{ abi::__cxa_demangle (
                    typeid (WindowInfo.info.x11.display).name (), nullptr,
                    nullptr, nullptr) },
                "]",
#else
                "  [",
                std::string_view{
                    typeid (WindowInfo.info.x11.display).name () },
                "]",
#endif
                PATATA_TERM_RESET,
#else  // PATATA_DEBUG
                "  ",
#endif // PATATA_DEBUG
                PATATA_TERM_BOLD, "The X11 display", PATATA_TERM_RESET);
        }
      break;
#endif

    case SDL_SYSWM_MIR:
      fast_io::io::println (
          PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET, "Mir",
#if PATATA_DEBUG == 1
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          " [(",
          std::string_view{ abi::__cxa_demangle (
              typeid (XDG_SESSION_TYPE).name (), nullptr, nullptr, nullptr) },
          ") XDG_SESSION_TYPE]",
#else
          " [(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
          ") XDG_SESSION_TYPE]",
#endif
          PATATA_TERM_RESET,
#endif // PATATA_DEBUG
          " | ", std::string_view{ XDG_SESSION_TYPE });
      break;

    default:
      fast_io::io::println (
          PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET, "Unknown",
#if PATATA_DEBUG == 1
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          " [(",
          std::string_view{ abi::__cxa_demangle (
              typeid (XDG_SESSION_TYPE).name (), nullptr, nullptr, nullptr) },
          ") XDG_SESSION_TYPE]",
#else
          " [(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
          ") XDG_SESSION_TYPE]",
#endif
          PATATA_TERM_RESET,
#endif // PATATA_DEBUG
          " | ", std::string_view{ XDG_SESSION_TYPE });
      break;
    }

  fast_io::io::println (PATATA_TERM_BOLD,
                        "  Window creation flags :", PATATA_TERM_RESET);

  uint32_t WindowFlags = SDL_GetWindowFlags (GameWindow);
#if PATATA_DEBUG == 1
  engineInfo.WindowCreationFlags = WindowFlags;
#endif

  if (WindowFlags & SDL_WINDOW_FULLSCREEN)
    fast_io::io::println ("    fullscreen window");

  if (WindowFlags & SDL_WINDOW_OPENGL)
    fast_io::io::println ("    window usable with OpenGL context");

  if (WindowFlags & SDL_WINDOW_SHOWN)
    fast_io::io::println ("    window is visible");

  if (WindowFlags & SDL_WINDOW_HIDDEN)
    fast_io::io::println ("    window is not visible");

  if (WindowFlags & SDL_WINDOW_BORDERLESS)
    fast_io::io::println ("    no window decoration");

  if (WindowFlags & SDL_WINDOW_RESIZABLE)
    fast_io::io::println ("    window can be resized");

  if (WindowFlags & SDL_WINDOW_MINIMIZED)
    fast_io::io::println ("    window is minimized");

  if (WindowFlags & SDL_WINDOW_MAXIMIZED)
    fast_io::io::println ("    window is maximized");

  if (WindowFlags & SDL_WINDOW_MOUSE_GRABBED)
    fast_io::io::println ("    window has grabbed mouse input");

  if (WindowFlags & SDL_WINDOW_INPUT_FOCUS)
    fast_io::io::println ("    window has input focus");

  if (WindowFlags & SDL_WINDOW_MOUSE_FOCUS)
    fast_io::io::println ("    window has mouse focus");

  if (WindowFlags & SDL_WINDOW_FOREIGN)
    fast_io::io::println ("    window not created by SDL");

  if (WindowFlags & SDL_WINDOW_ALLOW_HIGHDPI)
    fast_io::io::println (
        "    window should be created in high-DPI mode if supported");

  if (WindowFlags & SDL_WINDOW_MOUSE_CAPTURE)
    fast_io::io::println (
        "    window has mouse captured (unrelated to MOUSE_GRABBED)");

  if (WindowFlags & SDL_WINDOW_ALWAYS_ON_TOP)
    fast_io::io::println ("    window should always be above others");

  if (WindowFlags & SDL_WINDOW_SKIP_TASKBAR)
    fast_io::io::println ("    window should not be added to the taskbar");

  if (WindowFlags & SDL_WINDOW_UTILITY)
    fast_io::io::println ("    window should be treated as a utility window");

  if (WindowFlags & SDL_WINDOW_TOOLTIP)
    fast_io::io::println ("    window should be treated as a tooltip");

  if (WindowFlags & SDL_WINDOW_POPUP_MENU)
    fast_io::io::println ("    window should be treated as a popup menu");

  if (WindowFlags & SDL_WINDOW_KEYBOARD_GRABBED)
    fast_io::io::println ("    window has grabbed keyboard input");

  if (WindowFlags & SDL_WINDOW_VULKAN)
    fast_io::io::println ("    window usable for Vulkan surface");

  if (WindowFlags & SDL_WINDOW_METAL)
    fast_io::io::println ("    window usable for Metal view");

  fast_io::io::println ("");
}
