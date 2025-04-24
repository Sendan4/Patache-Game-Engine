// cstring wrapped

#if defined(_MSC_VER) // Windows MSVC
// strncpy
#define PATATA_STRNCPY(DEST, SRC, SIZE) strncpy_s (DEST, SRC, SIZE)

// strncat
#define PATATA_STRNCAT(DEST, SRC, SIZE) strncat_s (DEST, SRC, SIZE)

// memcpy
#define PATATA_MEMCPY(DEST, SRC, SIZE) memcpy_s (DEST, SIZE, SRC, SIZE)
#else // UNIX or (GCC or Clang)
// strncpy
#define PATATA_STRNCPY(DEST, SRC, SIZE) std::strncpy (DEST, SRC, SIZE)

// strncat
#define PATATA_STRNCAT(DEST, SRC, SIZE) std::strncat (DEST, SRC, SIZE)

// memcpy
#define PATATA_MEMCPY(DEST, SRC, SIZE)  std::memcpy (DEST, SRC, SIZE)
#endif
