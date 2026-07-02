#ifndef LIBEXPORT_HPP
#define LIBEXPORT_HPP
#if defined(_WIN64)
#  ifdef SHARED_LIBRARY_EXPORT_DEFINE
#    define PATACHE_API __declspec (dllexport)
#  else
#    define PATACHE_API
#  endif
#elif PATACHE_LINUX_OR_UNIX
#  define PATACHE_API __attribute__ ((visibility ("default")))
#endif
#endif
