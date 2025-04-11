#include "StartLogInfo.hpp"

void
Patata::Log::StartPatataLogInfo (void)
{
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_PATATA,
                        PATATA_ENGINE_NAME, PATATA_TERM_RESET, " INFO");

#if PATATA_DEBUG == 1
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if defined(PATATA_GIT_BRANCH)
      PATATA_TERM_BOLD, fast_io::mnp::right ("Branch : ", 11),
      PATATA_TERM_RESET, PATATA_GIT_BRANCH, "\n",
#endif
#if defined(PATATA_GIT_HASH_LONG) && defined(PATATA_GIT_HASH_SHORT)
      PATATA_TERM_BOLD, fast_io::mnp::right ("Commit Hash : ", 16), "\n",
      PATATA_TERM_RESET, "    ", PATATA_GIT_HASH_LONG, "\n    ",
      PATATA_GIT_HASH_SHORT, "\n",
#if defined(PATATA_GIT_WORK_DIR_IS_CLEAN)
#if PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
      PATATA_TERM_COLOR_GREEN,
#else
      PATATA_TERM_COLOR_YELLOW,
#endif
      "    ", PATATA_GIT_WORK_DIR_IS_CLEAN,
#endif
#if defined(PATATA_GIT_WORK_DIR_IS_STAGED)
      PATATA_TERM_RESET, " | ",
#if PATATA_GIT_WORK_DIR_IS_STAGED_BOOL == 1
      PATATA_TERM_COLOR_GREEN,
#else
      PATATA_TERM_COLOR_YELLOW,
#endif
      PATATA_GIT_WORK_DIR_IS_STAGED,
#endif
      PATATA_TERM_RESET
#endif
  );
#endif // PATATA_DEBUG

  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
      // Patata Version
      PATATA_TERM_BOLD, fast_io::mnp::right ("Version : ", 12),
      PATATA_TERM_RESET, PATATA_ENGINE_VERSION, " \n",
      // Patata Build Date
      PATATA_TERM_BOLD, fast_io::mnp::right ("Build Date : ", 15),
      PATATA_TERM_RESET, __DATE__, " | ", __TIME__, " \n",
      // Patata Compiler
      PATATA_TERM_BOLD, fast_io::mnp::right ("Compiler : ", 13),
      PATATA_TERM_RESET, PATATA_TERM_DIM, PATATA_COMPILER_PROGRAM,
      PATATA_TERM_RESET, " | ", PATATA_TERM_RESET, PATATA_COMPILER, " ",
      PATATA_COMPILER_VERSION, " \n",
      // Patata Build System
      PATATA_TERM_BOLD, fast_io::mnp::right ("Build System : ", 17),
      PATATA_TERM_RESET, PATATA_TERM_DIM, PATATA_BUILD_SYSTEM, " ",
      PATATA_BUILD_SYSTEM_VERSION, PATATA_TERM_RESET, " | ",
      PATATA_BUILD_SYSTEM_GENERATOR, " ",
#if defined(PATATA_BUILD_SYSTEM_GENERATOR_VERSION)
      PATATA_BUILD_SYSTEM_GENERATOR_VERSION, " \n",
#else
      " \n",
#endif
      // Patata Build Type
      PATATA_TERM_BOLD, fast_io::mnp::right ("Build Type : ", 15),
      PATATA_TERM_RESET, PATATA_BUILD_TYPE, " \n",
      // Patata OS
      PATATA_TERM_BOLD, fast_io::mnp::right ("Operating System : ", 21),
      PATATA_TERM_RESET, PATATA_OS, " \n",
      // CPU Arch
      PATATA_TERM_BOLD, fast_io::mnp::right ("CPU Architecture : ", 21),
      PATATA_TERM_RESET, PATATA_ARCH);

  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT);
}
