#include <fast_io.h>

// Patache Engine
#include "StartLogInfo.hpp"
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"

void
StartLogInfo (void)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_PATACHE, PATACHE_ENGINE_NAME,
                        PATACHE_TERM_RESET, " INFO");

#if PATACHE_DEBUG == 1
  #if defined(PATACHE_USE_GIT)
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Branch : ", 11), PATACHE_TERM_RESET,
                        PATACHE_GIT_BRANCH, "\n", PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Commit Hash : ", 16), "\n", PATACHE_TERM_RESET,
                        "    ", PATACHE_GIT_HASH_LONG, "\n    ", PATACHE_GIT_HASH_SHORT, "\n",
    #if PATACHE_GIT_DIRTY == 1
                        PATACHE_TERM_COLOR_YELLOW, fast_io::mnp::right ("Dirty", 9),
    #else
                        PATACHE_TERM_COLOR_GREEN, fast_io::mnp::right ("Clean", 10),
    #endif
                        PATACHE_TERM_RESET, " | ",
    #if PATACHE_GIT_STAGE == 1
                        PATACHE_TERM_COLOR_GREEN,
                        "In stage area"
    #else
                        PATACHE_TERM_COLOR_YELLOW,
                        "Un-staged changes"
    #endif
                        PATACHE_TERM_RESET);
  #endif
#endif // PATACHE_DEBUG

  fast_io::io::println (
      PATACHE_FASTIO_BUFFOUT,
      // Patache Version
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Version : ", 12), PATACHE_TERM_RESET,
      PATACHE_ENGINE_VERSION, " \n",
      // Patache Build Date
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build Date : ", 15), PATACHE_TERM_RESET, __DATE__,
      " | ", __TIME__, " \n",
      // Patache Compiler
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Compiler : ", 13), PATACHE_TERM_RESET,
      PATACHE_TERM_DIM, PATACHE_COMPILER_PROGRAM, PATACHE_TERM_RESET, " | ", PATACHE_TERM_RESET,
      PATACHE_COMPILER, " ", PATACHE_COMPILER_VERSION, " \n",
      // Patache Build System
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build System : ", 17), PATACHE_TERM_RESET,
      PATACHE_TERM_DIM, PATACHE_BUILD_SYSTEM, " ", PATACHE_BUILD_SYSTEM_VERSION, PATACHE_TERM_RESET,
      " | ", PATACHE_BUILD_SYSTEM_GENERATOR, " ",
#if defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
      PATACHE_BUILD_SYSTEM_GENERATOR_VERSION, " \n",
#else
      " \n",
#endif
      // Patache Build Type
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build Type : ", 15), PATACHE_TERM_RESET,
      PATACHE_BUILD_TYPE, " \n",
      // Patache OS
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Operating System : ", 21), PATACHE_TERM_RESET,
      PATACHE_OS, " \n",
      // CPU Arch
      PATACHE_TERM_BOLD, fast_io::mnp::right ("CPU Architecture : ", 21), PATACHE_TERM_RESET,
      PATACHE_ARCH);

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}
