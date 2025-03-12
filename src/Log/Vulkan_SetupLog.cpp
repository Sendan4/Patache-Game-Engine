#include "Vulkan_SetupLog.hpp"

void
Patata::Log::VulkanList (const char * const List[], const uint32_t & Size,
                         const char * const Message)
{
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, "[",
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (typeid (*List).name (),
                                                   nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (*List).name ()),
#endif
      "] ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, fast_io::mnp::os_c_str (Message),
      " : ", PATATA_TERM_RESET, Size);

  for (std::uint32_t i = 0; i < Size; ++i)
    {
      if (List[i] != nullptr)
        fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, "  ", PATATA_TERM_DIM,
                              fast_io::mnp::os_c_str (List[i]),
                              PATATA_TERM_RESET);
      else
        break;
    }

  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT);
}

void
Patata::Log::VulkanCheck (const char * const Message,
                          const vk::Result & Result)
{
  if (Result != vk::Result::eSuccess && Result != vk::Result::eSuboptimalKHR)
    {
      fast_io::io::println (
          PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, "[",
#if defined(__GNUC__)                                                         \
    || defined(__MINGW64__) && !defined(__clang__) && defined(DEBUG)
          fast_io::mnp::os_c_str (abi::__cxa_demangle (
              typeid (Result).name (), nullptr, nullptr, nullptr)),
#else
          fast_io::mnp::os_c_str (typeid (Result).name ()),
#endif
          "] ",
#endif // PATATA_DEBUG
          PATATA_TERM_RESET, PATATA_TERM_BOLD,
          fast_io::mnp::os_c_str (Message), " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, vk::to_string (Result), PATATA_TERM_RESET);
    }
  else
    fast_io::io::println (
        PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, "[",
#if defined(__GNUC__)                                                         \
    || defined(__MINGW64__) && !defined(__clang__) && defined(DEBUG)
        fast_io::mnp::os_c_str (abi::__cxa_demangle (
            typeid (Result).name (), nullptr, nullptr, nullptr)),
#else
        fast_io::mnp::os_c_str (typeid (Result).name ()),
#endif
        "] ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, fast_io::mnp::os_c_str (Message),
        " : ", PATATA_TERM_RESET, PATATA_TERM_COLOR_GREEN,
        vk::to_string (Result), PATATA_TERM_RESET);
}
