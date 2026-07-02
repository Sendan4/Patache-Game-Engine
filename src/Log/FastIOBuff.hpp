#ifndef FASTIOBUFF_HPP
#define FASTIOBUFF_HPP

#if defined(_WIN64)
  #define PATACHE_FASTIO_BUFFOUT fast_io::out ()
#else
  #define PATACHE_FASTIO_BUFFOUT fast_io::c_stdout ()
#endif

#endif
