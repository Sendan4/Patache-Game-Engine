// cstdlib wrapped

#if defined(_WIN64) // Windows MSVC
// setenv
#define PATATA_SETENV(VARIABLE, VALUE) SetEnvironmentVariable (VARIABLE, VALUE)
#define PATATA_SETENV_SUCCESS          1
#else // UNIX or (GCC or Clang)
// setenv
#define PATATA_SETENV(VARIABLE, VALUE) setenv (VARIABLE, VALUE, 1)
#define PATATA_SETENV_SUCCESS          0
#endif
