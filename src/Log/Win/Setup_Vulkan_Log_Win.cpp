#include "Setup_Vulkan_Log.hpp"

void
Patata::Log::VulkanList (const char * List[], const std::size_t & ListSize,
                         const std::string & MESSAGE)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

#if defined(__GNUC__) || defined(__MINGW64__)
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
                      std::string_view{ abi::__cxa_demangle (
                          typeid (*List).name (), nullptr, nullptr, nullptr) },
                      "] ");
#else
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
                      std::string_view{ typeid (*List).name () }, "] ");
#endif

  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_WHITE, MESSAGE,
                        " : ", PATATA_TERM_COLOR_GRAY1, ListSize + 1);

  for (std::size_t i = 0; i <= ListSize; i++)
    {
      if (List[i] != nullptr)
        fast_io::io::println (fast_io::out (), "  ",
                              std::string_view{ List[i] });
      else
        break;
    }

  fast_io::io::println (fast_io::out (), PATATA_TERM_RESET);
  SetConsoleMode (Terminal, mode);
}

void
Patata::Log::VulkanCheck (const std::string & Message,
                          const vk::Result &  Result)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

#if defined(__GNUC__) || defined(__MINGW64__)
  fast_io::io::print (
      fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
      std::string_view{ abi::__cxa_demangle (typeid (Result).name (), nullptr,
                                             nullptr, nullptr) },
      "] ");
#else
  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[",
                      std::string_view{ typeid (Result).name () }, "] ");
#endif

  fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_WHITE, Message,
                      " : ");

  if (Result != vk::Result::eSuccess)
    fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_YELLOW);
  else
    fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN);

  fast_io::io::println (fast_io::out (), vk::to_string (Result),
                        PATATA_TERM_RESET);

  SetConsoleMode (Terminal, mode);
}
