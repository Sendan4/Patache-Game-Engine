#if defined(_WIN64)
#  ifdef SHARED_LIBRARY_EXPORT_DEFINE
#    define PATACHE_API __declspec (dllexport)
#  else
#    define PATACHE_API
#  endif
#elif __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__  \
    || __DragonFly__ || __MidnightBSD__
#  define PATACHE_API __attribute__ ((visibility ("default")))
#endif
