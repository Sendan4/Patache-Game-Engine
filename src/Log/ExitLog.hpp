namespace Patata
{
namespace Log
{
template <typename pointer>
void
DeleteAndLogPtr (const std::string & Message, pointer * ptr)
{
  if (ptr != nullptr)
    {
      delete ptr;

      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_GREEN, "Deleted", PATATA_TERM_RESET);
    }
  else
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Not deleted", PATATA_TERM_RESET, " : ",
          fast_io::mnp::pointervw (ptr));
    }
}

template <typename pointer>
void
DeleteAndLogArrPtr (const std::string & Message, pointer * ptr)
{
  if (ptr != nullptr)
    {
      delete[] ptr;

      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_GREEN, "Deleted", PATATA_TERM_RESET);
    }
  else
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Not deleted", PATATA_TERM_RESET, " : ",
          fast_io::mnp::pointervw (ptr));
    }
}
}
}
