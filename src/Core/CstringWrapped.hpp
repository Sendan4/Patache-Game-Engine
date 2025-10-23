// cstring wrapped

#if defined(_MSC_VER) // Windows MSVC
  // strncpy
  #define PATACHE_STRNCPY(DEST, SRC, SIZE, COUNT) strncpy_s (DEST, SIZE, SRC, COUNT)

  // strncat
  #define PATACHE_STRNCAT(DEST, SRC, SIZE, COUNT) strncat_s (DEST, SIZE, SRC, COUNT)

  // memcpy
  #define PATACHE_MEMCPY(DEST, SRC, SIZE, COUNT) memcpy_s (DEST, SIZE, SRC, COUNT)
#else // UNIX or (GCC or Clang)
  // strncpy
  #define PATACHE_STRNCPY(DEST, SRC, SIZE, COUNT) std::strncpy (DEST, SRC, SIZE)

  // strncat
  #define PATACHE_STRNCAT(DEST, SRC, SIZE, COUNT) std::strncat (DEST, SRC, SIZE)

  // memcpy
  #define PATACHE_MEMCPY(DEST, SRC, SIZE, COUNT)  std::memcpy (DEST, SRC, SIZE)
#endif
