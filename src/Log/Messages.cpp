#include "Messages.hpp"

void
Patache::Log::FatalErrorMessage (const char * const     Title,
                                const char * const     Message,
                                const Patache::Config & Config)
{
  char ErrorTitle[1024] = "Patache Engine - ";

  PATACHE_STRNCPY (ErrorTitle, Title, 1024);

  if (Config.ShowFatalErrorMessagebox)
    SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, ErrorTitle, Message,
                              nullptr);
  else
    {
#if defined(_WIN64)
      HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
      DWORD  mode     = 0;
      GetConsoleMode (Terminal, &mode);
      SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

      fast_io::io::perrln (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                           PATACHE_TERM_COLOR_RED,
                           "ERROR FATAL : ", PATACHE_TERM_RESET,
                           fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
      SetConsoleMode (Terminal, mode);
#endif
    }
}

void
Patache::Log::ErrorMessage (const char * const Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                       PATACHE_TERM_COLOR_RED, "ERROR : ", PATACHE_TERM_RESET,
                       fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}

void
Patache::Log::WarningMessage (const char * const Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (
      PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_YELLOW,
      "WARNING : ", PATACHE_TERM_RESET, fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}
