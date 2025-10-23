// OS
#if __linux__ && !__ANDROID__
#define PATACHE_OS "GNU/Linux"
#elif _WIN64 || _WIN32
#define PATACHE_OS "Windows"
#elif __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__  \
    || __DragonFly__ || __MidnightBSD__
#define PATACHE_OS "Unix BSD"
#else
#define PATACHE_OS "undefined"
#endif

// Compiler
#if __GNUC__
#if __clang__
#define PATACHE_COMPILER "Clang"
#elif __MINGW64__
#define PATACHE_COMPILER "Minimalist GNU for Windows (MinGW-w64)"
#else
#define PATACHE_COMPILER "GNU Compiler Collection (GCC)"
#endif
#elif _MSC_VER <= 1929
#define PATACHE_COMPILER "Old Microsoft Visual C++ (MSVC)"
#elif _MSC_VER >= 1930
#define PATACHE_COMPILER "Microsoft Visual C++ (MSVC)"
#else
#define PATACHE_COMPILER "undefined"
#endif

void StartLogInfo (void);
