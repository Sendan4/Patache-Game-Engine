#include "WindowLog.hpp"

void
Patata::Log::WindowLog (SDL_Window * Window)
{
  fast_io::io::println (
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (Window).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (Window).name () }, "] ",
#endif
      PATATA_TERM_COLOR_PATATA, PATATA_TERM_BOLD, "Window ", PATATA_TERM_RESET,
      "INFO");

  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (Window, &WindowInfo);
  const char * XDG_SESSION_TYPE = getenv ("XDG_SESSION_TYPE");

  switch (WindowInfo.subsystem)
    {
    case SDL_SYSWM_WAYLAND:
      fast_io::io::println (
          PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET,
          "Wayland ", PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[(",
          std::string_view{ abi::__cxa_demangle (
              typeid (XDG_SESSION_TYPE).name (), nullptr, nullptr, nullptr) },
          ") XDG_SESSION_TYPE] ",
#else
          "[(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
          ") XDG_SESSION_TYPE] ",
#endif
          PATATA_TERM_RESET, std::string_view{ XDG_SESSION_TYPE });

      fast_io::io::println (PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                            "  [",
                            std::string_view{ abi::__cxa_demangle (
                                typeid (WindowInfo.info.wl.display).name (),
                                nullptr, nullptr, nullptr) },
                            "]",
#else
                            "  [",
                            std::string_view{
                                typeid (WindowInfo.info.wl.display).name () },
                            "]",
#endif
                            PATATA_TERM_RESET, PATATA_TERM_BOLD,
                            " Window Type", PATATA_TERM_RESET);

      fast_io::io::println (PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                            "  [",
                            std::string_view{ abi::__cxa_demangle (
                                typeid (WindowInfo.info.wl.surface).name (),
                                nullptr, nullptr, nullptr) },
                            "]",
#else
                            "  [",
                            std::string_view{
                                typeid (WindowInfo.info.wl.surface).name () },
                            "]",
#endif
                            PATATA_TERM_RESET, PATATA_TERM_BOLD,
                            " Surface Type", PATATA_TERM_RESET);
      break;

#if defined(PATATA_LINUX_XORG_SUPPORT) && defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
      if (strcmp (XDG_SESSION_TYPE, "wayland") == 0)
        fast_io::io::println (
            PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET,
            "XWayland", PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            " [(",
            std::string_view{
                abi::__cxa_demangle (typeid (XDG_SESSION_TYPE).name (),
                                     nullptr, nullptr, nullptr) },
#else
            " [(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
#endif
            ") XDG_SESSION_TYPE] ", PATATA_TERM_RESET,
            std::string_view{ XDG_SESSION_TYPE });
      else
        {
          fast_io::io::println (PATATA_TERM_BOLD,
                                "  Window System : ", PATATA_TERM_RESET, "X",
                                PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                                " [(",
                                std::string_view{ abi::__cxa_demangle (
                                    typeid (XDG_SESSION_TYPE).name (), nullptr,
                                    nullptr, nullptr) },
#else
                                " [(",
                                std::string_view{
                                    typeid (XDG_SESSION_TYPE).name () },
#endif
                                ") XDG_SESSION_TYPE] ", PATATA_TERM_RESET,
                                std::string_view{ XDG_SESSION_TYPE });

          fast_io::io::println (
              PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
              "  [",
              std::string_view{ abi::__cxa_demangle (
                  typeid (WindowInfo.info.x11.window).name (), nullptr,
                  nullptr, nullptr) },
              "]",
#else
              "  [",
              std::string_view{ typeid (WindowInfo.info.x11.window).name () },
              "]",
#endif
              PATATA_TERM_RESET, PATATA_TERM_BOLD, " Window Type",
              PATATA_TERM_RESET);

          fast_io::io::println (
              PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
              "  [",
              std::string_view{ abi::__cxa_demangle (
                  typeid (WindowInfo.info.x11.display).name (), nullptr,
                  nullptr, nullptr) },
              "]",
#else
              "  [",
              std::string_view{ typeid (WindowInfo.info.x11.display).name () },
              "]",
#endif
              PATATA_TERM_RESET, PATATA_TERM_BOLD, " Surface Type",
              PATATA_TERM_RESET);
        }
      break;
#endif

    case SDL_SYSWM_MIR:
      fast_io::io::println (
          PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET, "Mir",
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          " [(",
          std::string_view{ abi::__cxa_demangle (
              typeid (XDG_SESSION_TYPE).name (), nullptr, nullptr, nullptr) },
#else
          " [(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
#endif
          ") XDG_SESSION_TYPE] ", PATATA_TERM_RESET,
          std::string_view{ XDG_SESSION_TYPE });
      break;

    default:
      fast_io::io::println (
          PATATA_TERM_BOLD, "  Window System : ", PATATA_TERM_RESET, "Unknown",
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          " [(",
          std::string_view{ abi::__cxa_demangle (
              typeid (XDG_SESSION_TYPE).name (), nullptr, nullptr, nullptr) },
#else
          " [(", std::string_view{ typeid (XDG_SESSION_TYPE).name () },
#endif
          ") XDG_SESSION_TYPE] ", PATATA_TERM_RESET,
          std::string_view{ XDG_SESSION_TYPE });
      break;
    }

  const char * XDG_CURRENT_DESKTOP = getenv ("XDG_CURRENT_DESKTOP");

  if (XDG_CURRENT_DESKTOP != nullptr)
    fast_io::io::println (
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "  [(",
        std::string_view{ abi::__cxa_demangle (
            typeid (XDG_CURRENT_DESKTOP).name (), nullptr, nullptr, nullptr) },
#else
        "  [(", std::string_view{ typeid (XDG_CURRENT_DESKTOP).name () },
#endif
        ") XDG_CURRENT_DESKTOP]", PATATA_TERM_RESET, PATATA_TERM_BOLD,
        " Desktop : ", PATATA_TERM_RESET,
        std::string_view{ XDG_CURRENT_DESKTOP }, "\n");
}
