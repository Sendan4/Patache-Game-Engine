#if defined(_WIN64)
#define PATATA_FAST_IO_BUFF_OUT fast_io::out ()
#else
#define PATATA_FAST_IO_BUFF_OUT fast_io::c_stdout ()
#endif
