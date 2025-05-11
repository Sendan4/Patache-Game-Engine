// Foreground
#define PATACHE_TERM_COLOR_PATACHE "\033[38;5;180m"
#define PATACHE_TERM_COLOR_RED     "\033[38;5;203m"
#define PATACHE_TERM_COLOR_BLUE    "\033[38;5;75m"
#define PATACHE_TERM_COLOR_GREEN   "\033[38;5;118m"
#define PATACHE_TERM_COLOR_YELLOW  "\033[38;5;227m"
#define PATACHE_TERM_COLOR_GRAY0   "\033[38;5;245m"
#define PATACHE_TERM_COLOR_GRAY1   "\033[38;5;250m"

// Others
// Bold And Dim not supported in Windows
#if defined(_WIN64)
#define PATACHE_TERM_DIM  ""
#define PATACHE_TERM_BOLD "\033[38;5;15m"
#else
#define PATACHE_TERM_BOLD "\033[1m"
#define PATACHE_TERM_DIM  "\033[2m"
#endif
#define PATACHE_TERM_RESET "\033[0m"
