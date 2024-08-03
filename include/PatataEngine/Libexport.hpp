#if defined(_WIN64)
#ifdef SHARED_LIBRARY_EXPORT_DEFINE
#define PATATA_API __declspec (dllexport)
#else
#define PATATA_API
#endif
#elif defined(__linux__)
#define PATATA_API __attribute__ ((visibility ("default")))
#endif
