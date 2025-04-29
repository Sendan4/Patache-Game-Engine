#if defined(_WIN64)
#ifdef SHARED_LIBRARY_EXPORT_DEFINE
#define PATACHE_API __declspec (dllexport)
#else
#define PATACHE_API
#endif
#elif defined(__linux__)
#define PATACHE_API __attribute__ ((visibility ("default")))
#endif
