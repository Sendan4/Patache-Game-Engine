// cstdlib wrapped

#if defined(_WIN64) // Windows MSVC
// setenv
#define PATACHE_SETENV(VARIABLE, VALUE)                                       \
  SetEnvironmentVariable (VARIABLE, VALUE)
#define PATACHE_SETENV_SUCCESS 1
#else // UNIX or (GCC or Clang)
// setenv
#define PATACHE_SETENV(VARIABLE, VALUE) setenv (VARIABLE, VALUE, 1)
#define PATACHE_SETENV_SUCCESS          0
#endif
