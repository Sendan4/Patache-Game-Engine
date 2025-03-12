#include "Messages.hpp"

void
Patata::Log::FatalErrorMessage (const char * const     Title,
                                const char * const     Message,
                                const Patata::Config & Config)
{
  char PatataErrorTitle[1024] = "Patata Engine - ";

  PATATA_STRNCPY (PatataErrorTitle, Title, 1024);

  if (Config.ShowFatalErrorMessagebox)
    SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, PatataErrorTitle, Message,
                              nullptr);
  else
    {
#if defined(_WIN64)
      HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
      DWORD  mode     = 0;
      GetConsoleMode (Terminal, &mode);
      SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

      fast_io::io::perrln (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                           PATATA_TERM_COLOR_RED,
                           "ERROR FATAL : ", PATATA_TERM_RESET,
                           fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
      SetConsoleMode (Terminal, mode);
#endif
    }
}

void
Patata::Log::ErrorMessage (const char * const Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                       PATATA_TERM_COLOR_RED, "ERROR : ", PATATA_TERM_RESET,
                       fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}

void
Patata::Log::WarningMessage (const char * const Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (
      PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD, PATATA_TERM_COLOR_YELLOW,
      "WARNING : ", PATATA_TERM_RESET, fast_io::mnp::os_c_str (Message));

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}
