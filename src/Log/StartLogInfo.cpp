#include "StartLogInfo.hpp"

void
Patache::Log::StartPatacheLogInfo (void)
{
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_PATACHE,
                        PATACHE_ENGINE_NAME, PATACHE_TERM_RESET, " INFO");

#if PATACHE_DEBUG == 1
  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if defined(PATACHE_GIT_BRANCH)
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Branch : ", 11),
      PATACHE_TERM_RESET, PATACHE_GIT_BRANCH, "\n",
#endif
#if defined(PATACHE_GIT_HASH_LONG) && defined(PATACHE_GIT_HASH_SHORT)
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Commit Hash : ", 16), "\n",
      PATACHE_TERM_RESET, "    ", PATACHE_GIT_HASH_LONG, "\n    ",
      PATACHE_GIT_HASH_SHORT, "\n",
#if defined(PATACHE_GIT_WORK_DIR_IS_CLEAN)
#if PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
      PATACHE_TERM_COLOR_GREEN,
#else
      PATACHE_TERM_COLOR_YELLOW,
#endif
      "    ", PATACHE_GIT_WORK_DIR_IS_CLEAN,
#endif
#if defined(PATACHE_GIT_WORK_DIR_IS_STAGED)
      PATACHE_TERM_RESET, " | ",
#if PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL == 1
      PATACHE_TERM_COLOR_GREEN,
#else
      PATACHE_TERM_COLOR_YELLOW,
#endif
      PATACHE_GIT_WORK_DIR_IS_STAGED,
#endif
      PATACHE_TERM_RESET
#endif
  );
#endif // PATACHE_DEBUG

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
      // Patache Version
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Version : ", 12),
      PATACHE_TERM_RESET, PATACHE_ENGINE_VERSION, " \n",
      // Patache Build Date
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build Date : ", 15),
      PATACHE_TERM_RESET, __DATE__, " | ", __TIME__, " \n",
      // Patache Compiler
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Compiler : ", 13),
      PATACHE_TERM_RESET, PATACHE_TERM_DIM, PATACHE_COMPILER_PROGRAM,
      PATACHE_TERM_RESET, " | ", PATACHE_TERM_RESET, PATACHE_COMPILER, " ",
      PATACHE_COMPILER_VERSION, " \n",
      // Patache Build System
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build System : ", 17),
      PATACHE_TERM_RESET, PATACHE_TERM_DIM, PATACHE_BUILD_SYSTEM, " ",
      PATACHE_BUILD_SYSTEM_VERSION, PATACHE_TERM_RESET, " | ",
      PATACHE_BUILD_SYSTEM_GENERATOR, " ",
#if defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
      PATACHE_BUILD_SYSTEM_GENERATOR_VERSION, " \n",
#else
      " \n",
#endif
      // Patache Build Type
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Build Type : ", 15),
      PATACHE_TERM_RESET, PATACHE_BUILD_TYPE, " \n",
      // Patache OS
      PATACHE_TERM_BOLD, fast_io::mnp::right ("Operating System : ", 21),
      PATACHE_TERM_RESET, PATACHE_OS, " \n",
      // CPU Arch
      PATACHE_TERM_BOLD, fast_io::mnp::right ("CPU Architecture : ", 21),
      PATACHE_TERM_RESET, PATACHE_ARCH);

  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);
}
