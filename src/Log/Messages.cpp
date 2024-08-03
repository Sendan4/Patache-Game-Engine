#include "Messages.hpp"

void
Patata::Log::FatalErrorMessage (const std::string & Title,
                                const std::string & Message,
                                const YAML::Node &  Config)
{
  std::string PatataErrorTitle = "Patata Engine - ";
  PatataErrorTitle += Title;

  if (Config["patata-engine"]["show-fatal-error-messagebox"].as<bool> ())
    {
      SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR,
                                PatataErrorTitle.c_str (), Message.c_str (),
                                nullptr);
    }
  else
    {
#if defined(_WIN64)
      HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
      DWORD  mode     = 0;
      GetConsoleMode (Terminal, &mode);
      SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

      fast_io::io::perrln (
#if defined(_WIN64)
          fast_io::out (),
#endif
#if !defined(_WIN64)
          PATATA_TERM_BOLD,
#endif
          PATATA_TERM_COLOR_RED, "ERROR FATAL : ", PATATA_TERM_RESET, Message);

#if defined(_WIN64)
      SetConsoleMode (Terminal, mode);
#endif
    }
}

void
Patata::Log::ErrorMessage (const std::string & Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (
#if defined(_WIN64)
      fast_io::out (),
#endif
#if !defined(_WIN64)
      PATATA_TERM_BOLD,
#endif
      PATATA_TERM_COLOR_RED, "ERROR : ", PATATA_TERM_RESET, Message);

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}

void
Patata::Log::WarningMessage (const std::string & Message)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
#endif

  fast_io::io::perrln (
#if defined(_WIN64)
      fast_io::out (),
#endif
#if !defined(_WIN64)
      PATATA_TERM_BOLD,
#endif
      PATATA_TERM_COLOR_YELLOW, "WARNING : ", PATATA_TERM_RESET, Message);

#if defined(_WIN64)
  SetConsoleMode (Terminal, mode);
#endif
}
