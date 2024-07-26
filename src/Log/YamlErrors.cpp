#include "YamlErrors.hpp"

void
Patata::Log::YamlFileErrorMessage (void)
{
#if defined(_WIN64)
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

  fast_io::io::perr (fast_io::out (), "\n", PATATA_TERM_COLOR_RED,
                     "ERROR FATAL : ", PATATA_TERM_RESET);
#else
  fast_io::io::perr ("\n", PATATA_TERM_BOLD, PATATA_TERM_COLOR_RED,
                     "Error Fatal : ", PATATA_TERM_RESET);
#endif

#if defined(GAME_NAME)
  fast_io::io::perrln (fast_io::out (), "Cannot find ", GAME_CONFIG_FILE_NAME,
                       " file");

  SetConsoleMode (Terminal, mode);

  {
    std::string errMessage = "Cannot find ";
    errMessage += GAME_CONFIG_FILE_NAME;
    errMessage += " file";

    SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, "Missing Config File",
                              errMessage.c_str (), nullptr);
  }
#else
  fast_io::io::perrln ("Cannot find patata.yaml file");

  SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, "Config File",
                            "Cannot find patata.yaml", nullptr);
#endif
}
