#include "Setup_Vulkan_Log.hpp"

void
Patata::Log::VulkanList (const char * List[], const std::size_t & ListSize,
                         const std::string & MESSAGE)
{
  fast_io::io::println (
      PATATA_TERM_BOLD, PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (*List).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (*List).name () }, "] ",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, MESSAGE, " : ", PATATA_TERM_RESET,
      ListSize + 1);

  for (std::size_t i = 0; i <= ListSize; ++i)
    {
      if (List[i] != nullptr)
        fast_io::io::println ("  ", PATATA_TERM_DIM,
                              std::string_view{ List[i] }, PATATA_TERM_RESET);
      else
        break;
    }

  fast_io::io::println ("");
}

void
Patata::Log::VulkanCheck (const std::string & Message,
                          const vk::Result &  Result)
{
  fast_io::io::print (
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (Result).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (Result).name () }, "] ",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET);

  if (Result != vk::Result::eSuccess)
    fast_io::io::print (PATATA_TERM_COLOR_YELLOW);
  else
    fast_io::io::print (PATATA_TERM_COLOR_GREEN);

  fast_io::io::println (vk::to_string (Result), PATATA_TERM_RESET);
}
