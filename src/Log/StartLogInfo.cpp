#include <fast_io.h>

// Patache Engine
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"

#include "StartLogInfo.hpp"

void
Patache::StartLogInfo (void)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, Patache::sTermColorPatache, PATACHE_ENGINE_NAME,
                        Patache::sTermReset, " INFO");

#if PATACHE_DEBUG == 1
  #if defined(PATACHE_USE_GIT)
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, Patache::sTermBold,
                        fast_io::mnp::right ("Branch : ", 11), Patache::sTermReset,
                        PATACHE_GIT_BRANCH, "\n", Patache::sTermBold,
                        fast_io::mnp::right ("Commit Hash : ", 16), "\n", Patache::sTermReset,
                        "    ", PATACHE_GIT_HASH_LONG, "\n    ", PATACHE_GIT_HASH_SHORT, "\n",
    #if PATACHE_GIT_DIRTY == 1
                        Patache::sTermColorYellow, fast_io::mnp::right ("Dirty", 9),
    #else
                        Patache::sTermColorGreen, fast_io::mnp::right ("Clean", 10),
    #endif
                        Patache::sTermReset, " | ",
    #if PATACHE_GIT_STAGE == 1
                        Patache::sTermColorGreen,
                        "In stage area"
    #else
                        Patache::sTermColorYellow, "Un-staged changes",
    #endif
                        Patache::sTermReset);
  #endif
#endif // PATACHE_DEBUG

  fast_io::io::println (
      PATACHE_FASTIO_BUFFOUT,
      // Patache Version
      Patache::sTermBold, fast_io::mnp::right ("Version : ", 12), Patache::sTermReset,
      PATACHE_ENGINE_VERSION, " \n",
      // Patache Build Date
      Patache::sTermBold, fast_io::mnp::right ("Build Date : ", 15), Patache::sTermReset, __DATE__,
      " | ", __TIME__, " \n",
      // Patache Compiler
      Patache::sTermBold, fast_io::mnp::right ("Compiler : ", 13), Patache::sTermReset,
      Patache::sTermDim, PATACHE_COMPILER_PROGRAM, Patache::sTermReset, " | ", Patache::sTermReset,
      PATACHE_COMPILER, " ", PATACHE_COMPILER_VERSION, " \n",
      // Patache Build System
      Patache::sTermBold, fast_io::mnp::right ("Build System : ", 17), Patache::sTermReset,
      Patache::sTermDim, PATACHE_BUILD_SYSTEM, " ", PATACHE_BUILD_SYSTEM_VERSION,
      Patache::sTermReset, " | ", PATACHE_BUILD_SYSTEM_GENERATOR, " ",
#if defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
      PATACHE_BUILD_SYSTEM_GENERATOR_VERSION, " \n",
#else
      " \n",
#endif
      // Patache Build Type
      Patache::sTermBold, fast_io::mnp::right ("Build Type : ", 15), Patache::sTermReset,
      PATACHE_BUILD_TYPE, " \n",
      // Patache OS
      Patache::sTermBold, fast_io::mnp::right ("Operating System : ", 21), Patache::sTermReset,
      PATACHE_OS, " \n",
      // CPU Arch
      Patache::sTermBold, fast_io::mnp::right ("CPU Architecture : ", 21), Patache::sTermReset,
      PATACHE_ARCH);

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}
