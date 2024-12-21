#include "Setup_Vulkan_Log.hpp"

void
Patata::Log::VulkanList (const char * List[], const uint32_t & Size,
                         const std::string_view & Message)
{
  fast_io::io::println (
#if defined(DEBUG)
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "[",
      std::string_view{ abi::__cxa_demangle (typeid (*List).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "[", std::string_view{ typeid (*List).name () }, "] ",
#endif
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, std::string_view{ Message }, " : ",
      PATATA_TERM_RESET, Size);

  for (uint32_t i = 0; i < Size; ++i)
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
Patata::Log::VulkanCheck (const std::string_view & Message,
                          const vk::Result &       Result)
{
  if (Result != vk::Result::eSuccess && Result != vk::Result::eSuboptimalKHR)
    {
      fast_io::io::println (
#if defined(DEBUG)
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__)                                                         \
    || defined(__MINGW64__) && !defined(__clang__) && defined(DEBUG)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (Result).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#elif defined(DEBUG)
          "[", std::string_view{ typeid (Result).name () }, "] ",
#endif
#endif
          PATATA_TERM_RESET, PATATA_TERM_BOLD, std::string_view{ Message },
          " : ", PATATA_TERM_RESET, PATATA_TERM_COLOR_YELLOW,
          vk::to_string (Result), PATATA_TERM_RESET);
    }
  else
    fast_io::io::println (
#if defined(DEBUG)
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__)                                                         \
    || defined(__MINGW64__) && !defined(__clang__) && defined(DEBUG)
        "[",
        std::string_view{ abi::__cxa_demangle (typeid (Result).name (),
                                               nullptr, nullptr, nullptr) },
        "] ",
#elif defined(DEBUG)
        "[", std::string_view{ typeid (Result).name () }, "] ",
#endif
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, std::string_view{ Message },
        " : ", PATATA_TERM_RESET, PATATA_TERM_COLOR_GREEN,
        vk::to_string (Result), PATATA_TERM_RESET);
}
