#include <cstring>

#include <SDL3/SDL.h>
#include <fast_io.h>

#include "PatacheEngine/StructConfig.hpp"
#include "Message.hpp"
#include "FastIOBuff.hpp"
#include "CstringWrapped.hpp"
#include "ColorTerminal.hpp"

void
Patache::FatalErrorMessage (const char * const pTitle, const char * const pMessage,
                            const Patache::Config & rConfiguration)
{
  char errorTitle[1024] = "Patache Engine - ";

  PATACHE_STRNCPY (errorTitle, pTitle, 1023, 1024);

  if (rConfiguration.showFatalErrorMessagebox)
    {
      SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, errorTitle, pMessage, nullptr);
    }
  else
    {
      fast_io::io::perrln (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_RED,
                           "ERROR FATAL : ", PATACHE_TERM_RESET, fast_io::mnp::os_c_str (pMessage));
    }
}

void
Patache::ErrorMessage (const char * const pMessage)
{
  fast_io::io::perrln (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_RED,
                       "ERROR : ", PATACHE_TERM_RESET, fast_io::mnp::os_c_str (pMessage));
}

void
Patache::WarningMessage (const char * const pMessage)
{
  fast_io::io::perrln (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_YELLOW,
                       "WARNING : ", PATACHE_TERM_RESET, fast_io::mnp::os_c_str (pMessage));
}
