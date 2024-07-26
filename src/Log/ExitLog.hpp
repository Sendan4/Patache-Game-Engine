namespace Patata
{
namespace Log
{
template <typename pointer>
void
DeleteAndLogPtr (const std::string & Message, pointer *& ptr)
{
  if (ptr != nullptr)
    {
      delete ptr;
      ptr = nullptr;

#if defined(_WIN64)
      fast_io::io::print (
          fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ");
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ");
#endif

      fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_WHITE, Message,
                          " : ", PATATA_TERM_RESET);
#else
      fast_io::io::print (
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_RESET);

      fast_io::io::print (PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET);
#endif

      if (ptr == nullptr)
        {
#if defined(_WIN64)
          fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GREEN,
                                "Deleted", PATATA_TERM_RESET);
#else
          fast_io::io::println (PATATA_TERM_COLOR_GREEN, "Deleted",
                                PATATA_TERM_RESET);
#endif
        }
      else
        {
#if defined(_WIN64)
          fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_YELLOW,
                                "Has Not Been Deleted", PATATA_TERM_RESET);
#else
          fast_io::io::println (PATATA_TERM_COLOR_YELLOW,
                                "Has Not Been Deleted", PATATA_TERM_RESET);
#endif
        }
    }
  else
    {
#if defined(_WIN64)
      fast_io::io::print (
          fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (ptr).name (), nullptr,
                                                 nullptr, nullptr) },
          "] ");
#else
          "[", std::string_view{ typeid (ptr).name () }, "] ");
#endif

      fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_WHITE, Message,
                            PATATA_TERM_COLOR_YELLOW,
                            " Is Already Eliminated Or Null");
#else
      fast_io::io::println (
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (ptr).name (), nullptr,
                                                 nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (ptr).name () }, "] ",
#endif
          PATATA_TERM_RESET, PATATA_TERM_BOLD, Message, PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW,
          " Already Eliminated Or Null" PATATA_TERM_RESET);
#endif
    }
}

template <typename pointer>
void
DeleteAndLogArrPtr (const std::string & Message, pointer *& ptr)
{
  if (ptr != nullptr)
    {
      delete[] ptr;
      ptr = nullptr;

#if defined(_WIN64)
      fast_io::io::print (
          fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ");
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ");
#endif

      fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_WHITE, Message,
                          " : ", PATATA_TERM_RESET);
#else
      fast_io::io::print (
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (*ptr).name (),
                                                 nullptr, nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (*ptr).name () }, "] ",
#endif
          PATATA_TERM_RESET);

      fast_io::io::print (PATATA_TERM_BOLD, Message, " : ", PATATA_TERM_RESET);
#endif

      if (ptr == nullptr)
        {
#if defined(_WIN64)
          fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GREEN,
                                "Deleted", PATATA_TERM_RESET);
#else
          fast_io::io::println (PATATA_TERM_COLOR_GREEN, "Deleted",
                                PATATA_TERM_RESET);
#endif
        }
      else
        {
#if defined(_WIN64)
          fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_YELLOW,
                                "Has Not Been Deleted", PATATA_TERM_RESET);
#else
          fast_io::io::println (PATATA_TERM_COLOR_YELLOW,
                                "Has Not Been Deleted", PATATA_TERM_RESET);
#endif
        }
    }
  else
    {
#if defined(_WIN64)
      fast_io::io::print (
          fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (ptr).name (), nullptr,
                                                 nullptr, nullptr) },
          "] ");
#else
          "[", std::string_view{ typeid (ptr).name () }, "] ");
#endif

      fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_WHITE, Message,
                            PATATA_TERM_COLOR_YELLOW,
                            " Is Already Eliminated Or Null");
#else
      fast_io::io::println (
          PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (ptr).name (), nullptr,
                                                 nullptr, nullptr) },
          "] ",
#else
          "[", std::string_view{ typeid (ptr).name () }, "] ",
#endif
          PATATA_TERM_RESET, PATATA_TERM_BOLD, Message, PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, " Already Eliminated Or Null",
          PATATA_TERM_RESET);
#endif
    }
}
}
}
