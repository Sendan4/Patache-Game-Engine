// before to use check if PATACHE_DEBUG is defined
/*
 * Abstracts the particularities between compilers to obtain the type name.
 */
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) || defined(__clang__) // Is GCC
#define PATACHE_GET_VARTYPE_STRING(STR_SRC, VAR)                                                   \
  STR_SRC = abi::__cxa_demangle (typeid (VAR).name (), nullptr, nullptr, nullptr)

#define PATACHE_FREE_VARTYPE_STRING(STR_DST) std::free (STR_DST)

#define PATACHE_VARTYPE_STRING_PTR char *
#else // others compilers
#define PATACHE_GET_VARTYPE_STRING(STR_SRC, VAR) STR_SRC = typeid (VAR).name ()

#define PATACHE_FREE_VARTYPE_STRING(STR_DST)

#define PATACHE_VARTYPE_STRING_PTR const char *
#endif
#endif

#if PATACHE_DEBUG == 0
#define PATACHE_GET_VARTYPE_STRING(STR_SRC, VAR)
#define PATACHE_FREE_VARTYPE_STRING(STR_DST)
#endif

// Añadir el TERM_RESET
/*
 * it is usable when compiled in debug mode. In release mode, it only adds spaces.
 */
#if PATACHE_DEBUG == 1
#define PATACHE_FASTIO_SHOW_VARTYPE_STRING(STR_SRC, ALING_LEFT, ALING_RIGHT, EMPTY_SPACE_RIGHT)    \
  PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", ALING_LEFT),               \
      fast_io::mnp::os_c_str (STR_SRC), fast_io::mnp::left ("]", ALING_RIGHT), PATACHE_TERM_RESET
#else
#define PATACHE_FASTIO_SHOW_VARTYPE_STRING(STR_SRC, ALING_LEFT, ALING_RIGHT, EMPTY_SPACE_RIGHT)    \
  fast_io::mnp::right ("", EMPTY_SPACE_RIGHT)
#endif
