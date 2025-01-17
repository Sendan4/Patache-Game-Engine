#include "Start_Patata_Log_Info.hpp"

void
Patata::Log::StartPatataLogInfo (const Patata::EngineCreateInfo & Info)
{
  fast_io::io::println (PATATA_TERM_COLOR_PATATA, PATATA_TERM_BOLD,
                        PATATA_ENGINE_NAME, PATATA_TERM_RESET, " INFO");

#if defined(PATATA_GIT_BRANCH)
  fast_io::io::println (PATATA_TERM_BOLD, "  Branch : ", PATATA_TERM_RESET,
                        PATATA_GIT_BRANCH);
#endif

#if defined(PATATA_GIT_HASH_LONG) && defined(PATATA_GIT_HASH_SHORT)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "  Commit Hash : ", PATATA_TERM_RESET);

  fast_io::io::println ("    ", PATATA_GIT_HASH_LONG, "\n    ",
                        PATATA_GIT_HASH_SHORT);

#if defined(PATATA_GIT_WORK_DIR_IS_CLEAN)
#if PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
  fast_io::io::print (PATATA_TERM_COLOR_GREEN);
#else
  fast_io::io::print (PATATA_TERM_COLOR_YELLOW);
#endif
  fast_io::io::print ("    ", PATATA_GIT_WORK_DIR_IS_CLEAN);
#endif

#if defined(PATATA_GIT_WORK_DIR_IS_STAGED)
  fast_io::io::print (PATATA_TERM_RESET, " | ");

#if PATATA_GIT_WORK_DIR_IS_STAGED_BOOL == 1
  fast_io::io::print (PATATA_TERM_COLOR_GREEN);
#else
  fast_io::io::print (PATATA_TERM_COLOR_YELLOW);
#endif

  fast_io::io::print (PATATA_GIT_WORK_DIR_IS_STAGED);
#endif

  fast_io::io::println (PATATA_TERM_RESET);
#endif

  fast_io::io::println (
      PATATA_TERM_BOLD, "  Version : ", PATATA_TERM_RESET,
      PATATA_ENGINE_VERSION, "\n", PATATA_TERM_BOLD,
      "  Build Date : ", PATATA_TERM_RESET, __DATE__, " | ", __TIME__, "\n",
      PATATA_TERM_BOLD, "  Compiler : ", PATATA_TERM_RESET, PATATA_TERM_DIM,
      PATATA_COMPILER_PROGRAM, PATATA_TERM_RESET, " | ", PATATA_TERM_RESET,
      PATATA_COMPILER, " ", PATATA_COMPILER_VERSION, "\n", PATATA_TERM_BOLD,
      "  Build System : ", PATATA_TERM_RESET, PATATA_TERM_DIM,
      PATATA_BUILD_SYSTEM, " ", PATATA_BUILD_SYSTEM_VERSION, PATATA_TERM_RESET,
      " | ", PATATA_BUILD_SYSTEM_GENERATOR, " ",
      PATATA_BUILD_SYSTEM_GENERATOR_VERSION, "\n", PATATA_TERM_BOLD,
      "  Build Type : ", PATATA_TERM_RESET, PATATA_BUILD_TYPE,
      "\n" PATATA_TERM_BOLD, "  Operating System : ", PATATA_TERM_RESET,
      PATATA_OS, "\n", PATATA_TERM_BOLD,
      "  CPU Architecture Type : ", PATATA_TERM_RESET, PATATA_ARCH);
#if defined(PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "  Fast IO Commit Hash Short : ", PATATA_TERM_RESET,
                        PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT);
#endif
  fast_io::io::println (
      PATATA_TERM_BOLD, "  SDL Version : ", PATATA_TERM_RESET,
      SDL_MAJOR_VERSION, ".", SDL_MINOR_VERSION, ".", SDL_PATCHLEVEL);

#if defined(PATATA_RAPIDYAML_VERSION)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "  RapidYaml Version : ", PATATA_TERM_RESET,
                        PATATA_RAPIDYAML_VERSION);
#endif

  if (Info.gameName != nullptr)
    {
      fast_io::io::println (PATATA_TERM_BOLD,
                            "  Game Name : ", PATATA_TERM_RESET,
                            std::string_view{ Info.gameName });
    }

  fast_io::io::println ("");
}
