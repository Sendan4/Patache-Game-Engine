#include "Start_Patata_Log_Info.hpp"

void
Patata::Log::StartPatataLogInfo (const Patata::EngineCreateInfo & Info)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);
  UINT defaultOutputCP = GetConsoleOutputCP ();
  SetConsoleOutputCP (CP_UTF8);

  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_PATATA,
                        PATATA_ENGINE_NAME, PATATA_TERM_BOLD, " INFO : ");

#if defined(PATATA_GIT_BRANCH)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                        "  Branch : ", PATATA_TERM_COLOR_GRAY1,
                        PATATA_GIT_BRANCH, " ");
#endif

#if defined(PATATA_GIT_HASH_LONG) && defined(PATATA_GIT_HASH_SHORT)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD, "  Commit Hash: ");

  fast_io::io::println (fast_io::out (), "    ", PATATA_TERM_COLOR_GRAY1,
                        PATATA_GIT_HASH_LONG);

  fast_io::io::println (fast_io::out (), "    ", PATATA_TERM_COLOR_GRAY1,
                        PATATA_GIT_HASH_SHORT);

#if defined(PATATA_GIT_WORK_DIR_IS_CLEAN)
#if PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
  fast_io::io::print (fast_io::out (), "    ", PATATA_TERM_COLOR_GREEN);
#else
  fast_io::io::print (fast_io::out (), "    ", PATATA_TERM_COLOR_YELLOW);
#endif
  fast_io::io::print (fast_io::out (), PATATA_GIT_WORK_DIR_IS_CLEAN);
#endif

#if defined(PATATA_GIT_WORK_DIR_IS_STAGED)
  fast_io::io::print (fast_io::out (), PATATA_TERM_RESET, " | ");

#if PATATA_GIT_WORK_DIR_IS_STAGED_BOOL == 1
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN);
#else
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_YELLOW);
#endif
  fast_io::io::print (fast_io::out (), PATATA_GIT_WORK_DIR_IS_STAGED);
#endif

  fast_io::io::println (fast_io::out ());
#endif

  fast_io::io::println (
      fast_io::out (), PATATA_TERM_BOLD,
      "  Version : ", PATATA_TERM_COLOR_GRAY1, PATATA_ENGINE_VERSION, "\n",
      PATATA_TERM_BOLD, "  Build Date : ", PATATA_TERM_COLOR_GRAY1, __DATE__,
      " | ", __TIME__, "\n", PATATA_TERM_BOLD,
      "  Compiler : ", PATATA_TERM_COLOR_GRAY1, PATATA_COMPILER_PROGRAM, " | ",
      PATATA_COMPILER, " ", PATATA_COMPILER_VERSION, "\n", PATATA_TERM_BOLD,
      "  Build System : ", PATATA_TERM_COLOR_GRAY1, PATATA_BUILD_SYSTEM,
#if defined(PATATA_BUILD_SYSTEM_VERSION)
      " ", PATATA_BUILD_SYSTEM_VERSION,
#endif
      " | ", PATATA_BUILD_SYSTEM_GENERATOR,
#if defined(PATATA_BUILD_SYSTEM_GENERATOR_VERSION)
      " ", PATATA_BUILD_SYSTEM_GENERATOR_VERSION,
#endif
      "\n", PATATA_TERM_BOLD, "  Build Type : ", PATATA_TERM_COLOR_GRAY1,
      PATATA_BUILD_TYPE, "\n", PATATA_TERM_BOLD,
      "  Operating System : ", PATATA_TERM_COLOR_GRAY1, PATATA_OS, "\n",
      PATATA_TERM_BOLD, "  CPU Arch : ", PATATA_TERM_COLOR_GRAY1, PATATA_ARCH,
      "\n",
#if defined(PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT)
      PATATA_TERM_BOLD, "  Fast IO Commit Hash : ", PATATA_TERM_COLOR_GRAY1,
      PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT, "\n",
#endif
      PATATA_TERM_BOLD, "  SDL Version : ", PATATA_TERM_COLOR_GRAY1,
      SDL_MAJOR_VERSION, ".", SDL_MINOR_VERSION, ".", SDL_PATCHLEVEL);

#if defined(PATATA_RAPIDYAML_VERSION)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                        "  RapidYaml Version : ", PATATA_TERM_COLOR_GRAY1,
                        PATATA_RAPIDYAML_VERSION);
#endif

if (Info.gameName != nullptr)
    {
      fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                            "  Game Name : ", PATATA_TERM_COLOR_GRAY1,
                            std::string_view{ Info.gameName });
    }
  
  SetConsoleOutputCP (defaultOutputCP);
  fast_io::io::println (fast_io::out (), PATATA_TERM_RESET);
  SetConsoleMode (Terminal, mode);
}
