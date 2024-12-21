#include "WindowLog.hpp"

void
#if defined(DEBUG)
Patata::Log::WindowLog (SDL_Window *         Window,
                        Patata::EngineInfo * PatataEngineInfo)
#else
Patata::Log::WindowLog (SDL_Window * Window)
#endif
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

  fast_io::io::println (
      fast_io::out (),
#if defined(DEBUG)
      PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
    "[",
      std::string_view{ abi::__cxa_demangle (typeid (Window).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (Window).name () }, "] ",
#endif
      PATATA_TERM_RESET,
#endif
      PATATA_TERM_COLOR_PATATA, "Window ",
                        PATATA_TERM_BOLD, "INFO :");

  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (Window, &WindowInfo);

  #if defined(DEBUG)
  PatataEngineInfo->WindowInfo = WindowInfo;
  #endif

  switch (WindowInfo.subsystem)
    {
    case SDL_SYSWM_WINDOWS:
      fast_io::io::println (fast_io::out (),
                            "  Window System : ", PATATA_TERM_COLOR_GRAY1,
                            "Windows");

      // The window handle
      fast_io::io::println (fast_io::out (),
#if defined(DEBUG)
                            PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
      "  [",
                          std::string_view{ abi::__cxa_demangle (
                              typeid (WindowInfo.info.win.window).name (),
                              nullptr, nullptr, nullptr) },
                          "] ",
#else
          "  [",
          std::string_view{ typeid (WindowInfo.info.win.window).name () },
          "] ",
#endif
          PATATA_TERM_RESET,
#else
          "  ",
#endif
      PATATA_TERM_BOLD, "The window handle");

      // The window device context
      fast_io::io::println (fast_io::out (),
#if defined(DEBUG)
                            PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
                            "  [",
                            std::string_view{ abi::__cxa_demangle (
                                typeid (WindowInfo.info.win.hdc).name (),
                                nullptr, nullptr, nullptr) },
                            "] ",
#else
                            PATATA_TERM_COLOR_GRAY0, "  [",
                            std::string_view{ typeid (WindowInfo.info.win.hdc).name () },
                            "] ",
#endif
                            PATATA_TERM_RESET,
#else
                            "  ",
#endif
                            PATATA_TERM_BOLD,
                            "The window device context");

      // The instance handle
      fast_io::io::println (fast_io::out (),
#if defined(DEBUG)
                            PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
                             "  [",
                            std::string_view{ abi::__cxa_demangle (
                                typeid (WindowInfo.info.win.hinstance).name (),
                                nullptr, nullptr, nullptr) },
                            "] ",
#else
                            PATATA_TERM_COLOR_GRAY0, "  [",
                            std::string_view{ typeid (WindowInfo.info.win.hinstance).name () },
                            "] ", PATATA_TERM_RESET, PATATA_TERM_RESET,
#endif
                            PATATA_TERM_RESET,
#else
                            "  ",
#endif
                            PATATA_TERM_BOLD, "The instance handle");

      break;

    case SDL_SYSWM_WINRT:
      fast_io::io::print (fast_io::out (), PATATA_TERM_BOLD,
                          "  Window System : ", PATATA_TERM_COLOR_GRAY1,
                          "WinRT");
      break;

    default:
    case SDL_SYSWM_UNKNOWN:
      fast_io::io::print (fast_io::out (), PATATA_TERM_BOLD,
                          "  Window System : ", PATATA_TERM_COLOR_YELLOW,
                          "Unknown");
      break;
    }

  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                        "  Window creation flags :", PATATA_TERM_COLOR_GRAY1);

  uint32_t WindowFlags = SDL_GetWindowFlags (Window);
  #if defined(DEBUG)
  PatataEngineInfo->WindowCreationFlags = WindowFlags;
  #endif

  if (WindowFlags & SDL_WINDOW_FULLSCREEN)
    fast_io::io::println (fast_io::out (), "    fullscreen window");

  if (WindowFlags & SDL_WINDOW_OPENGL)
    fast_io::io::println (fast_io::out (), "    window usable with OpenGL context");

  if (WindowFlags & SDL_WINDOW_SHOWN)
    fast_io::io::println (fast_io::out (), "    window is visible");

  if (WindowFlags & SDL_WINDOW_HIDDEN)
    fast_io::io::println (fast_io::out (), "    window is not visible");

  if (WindowFlags & SDL_WINDOW_BORDERLESS)
    fast_io::io::println (fast_io::out (), "    no window decoration");

  if (WindowFlags & SDL_WINDOW_RESIZABLE)
    fast_io::io::println (fast_io::out (), "    window can be resized");

  if (WindowFlags & SDL_WINDOW_MINIMIZED)
    fast_io::io::println (fast_io::out (), "    window is minimized");

  if (WindowFlags & SDL_WINDOW_MAXIMIZED)
    fast_io::io::println (fast_io::out (), "    window is maximized");

  if (WindowFlags & SDL_WINDOW_MOUSE_GRABBED)
    fast_io::io::println (fast_io::out (), "    window has grabbed mouse input");

  if (WindowFlags & SDL_WINDOW_INPUT_FOCUS)
    fast_io::io::println (fast_io::out (), "    window has input focus");

  if (WindowFlags & SDL_WINDOW_MOUSE_FOCUS)
    fast_io::io::println (fast_io::out (), "    window has mouse focus");

  if (WindowFlags & SDL_WINDOW_FOREIGN)
    fast_io::io::println (fast_io::out (), "    window not created by SDL");

  if (WindowFlags & SDL_WINDOW_ALLOW_HIGHDPI)
    fast_io::io::println (fast_io::out (), "    window should be created in high-DPI mode if supported");

  if (WindowFlags & SDL_WINDOW_MOUSE_CAPTURE)
    fast_io::io::println (
        fast_io::out (),
        "    window has mouse captured (unrelated to MOUSE_GRABBED)");

  if (WindowFlags & SDL_WINDOW_ALWAYS_ON_TOP)
    fast_io::io::println (fast_io::out (),
                          "    window should always be above others");

  if (WindowFlags & SDL_WINDOW_SKIP_TASKBAR)
    fast_io::io::println (fast_io::out (),
                          "    window should not be added to the taskbar");

  if (WindowFlags & SDL_WINDOW_UTILITY)
    fast_io::io::println (fast_io::out (),
                          "    window should be treated as a utility window");

  if (WindowFlags & SDL_WINDOW_TOOLTIP)
    fast_io::io::println (fast_io::out (),
                          "    window should be treated as a tooltip");

  if (WindowFlags & SDL_WINDOW_POPUP_MENU)
    fast_io::io::println (fast_io::out (),
                          "    window should be treated as a popup menu");

  if (WindowFlags & SDL_WINDOW_KEYBOARD_GRABBED)
    fast_io::io::println (fast_io::out (),
                          "    window has grabbed keyboard input");

  if (WindowFlags & SDL_WINDOW_VULKAN)
    fast_io::io::println (fast_io::out (),
                          "    window usable for Vulkan surface");

  if (WindowFlags & SDL_WINDOW_METAL)
    fast_io::io::println (fast_io::out (), "    window usable for Metal view");

  fast_io::io::println (fast_io::out (), PATATA_TERM_RESET);
  SetConsoleMode (Terminal, mode);
}
