#include "WindowLog.hpp"

void
Patata::Log::WindowLog (SDL_Window * Window)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

#if defined(__GNUC__) || defined(__MINGW64__)
  fast_io::io::print (
      fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
      std::string_view{ abi::__cxa_demangle (typeid (Window).name (), nullptr,
                                             nullptr, nullptr) },
      "] ");
#else
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
                      std::string_view{ typeid (Window).name () }, "] ");
#endif

  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_PATATA, "Window ",
                        PATATA_TERM_COLOR_WHITE, "INFO :");

  SDL_SysWMinfo WindowInfo;
  SDL_VERSION (&WindowInfo.version);
  SDL_GetWindowWMInfo (Window, &WindowInfo);

  switch (WindowInfo.subsystem)
    {
    case SDL_SYSWM_WINDOWS:
      fast_io::io::println (fast_io::out (),
                            "  Window System : ", PATATA_TERM_COLOR_GRAY1,
                            "Windows");

#if defined(__GNUC__) || defined(__MINGW64__)
      fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "  [",
                          std::string_view{ abi::__cxa_demangle (
                              typeid (WindowInfo.info.win.window).name (),
                              nullptr, nullptr, nullptr) },
                          "]");
#else
      fast_io::io::print (
          fast_io::out (), PATATA_TERM_COLOR_GRAY0, "  [",
          std::string_view{ typeid (WindowInfo.info.win.window).name () },
          "]");
#endif

      fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_WHITE,
                            " Window Type");
      break;

    case SDL_SYSWM_WINRT:
      fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_WHITE,
                          "  Window System : ", PATATA_TERM_COLOR_GRAY1,
                          "WinRT");
      break;

    default:
    case SDL_SYSWM_UNKNOWN:
      fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_WHITE,
                          "  Window System : ", PATATA_TERM_COLOR_YELLOW,
                          "Unknown");
      break;
    }

  fast_io::io::println (fast_io::out (), PATATA_TERM_RESET);
  SetConsoleMode (Terminal, mode);
}
