#include "Setup_Vulkan_Log.hpp"

void
Patata::Log::VulkanList (const char * List[], const uint32_t & Size,
                         const std::string_view & Message)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

  fast_io::io::println (
      fast_io::out (),
#if PATATA_DEBUG == 1
      PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (*List).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (*List).name () }, "] ",
#endif
#endif
      PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_COLOR_GRAY1, Size);

  for (uint32_t i = 0; i < Size; ++i)
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
Patata::Log::VulkanCheck (const std::string_view & Message,
                          const vk::Result &       Result)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

  if (Result != vk::Result::eSuccess && Result != vk::Result::eSuboptimalKHR)
    {
      fast_io::io::println (
          fast_io::out (),
#if PATATA_DEBUG == 1
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (Result).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (Result).name () }, "] ",
#endif
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, vk::to_string (Result), PATATA_TERM_RESET);
    }
  else
    {
      fast_io::io::println (
          fast_io::out (),
#if PATATA_DEBUG == 1
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (Result).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (Result).name () }, "] ",
#endif
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_GREEN, vk::to_string (Result), PATATA_TERM_RESET);
    }

  SetConsoleMode (Terminal, mode);
}
