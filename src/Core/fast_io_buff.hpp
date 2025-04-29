#if defined(_WIN64)
#define PATACHE_FAST_IO_BUFF_OUT fast_io::out ()
#else
#define PATACHE_FAST_IO_BUFF_OUT fast_io::c_stdout ()
#endif
