// Foreground
#define PATATA_TERM_COLOR_PATATA "\033[38;5;180m"
#define PATATA_TERM_COLOR_RED    "\033[38;5;203m"
#define PATATA_TERM_COLOR_BLUE   "\033[38;5;75m"
#define PATATA_TERM_COLOR_GREEN  "\033[38;5;118m"
#define PATATA_TERM_COLOR_YELLOW "\033[38;5;227m"
#define PATATA_TERM_COLOR_GRAY0  "\033[38;5;245m"
#define PATATA_TERM_COLOR_GRAY1  "\033[38;5;250m"

// Others
// Bold And Dim not supported in Windows
#if defined(_WIN64)
#define PATATA_TERM_DIM  ""
#define PATATA_TERM_BOLD "\033[38;5;15m"
#else
#define PATATA_TERM_BOLD "\033[1m"
#define PATATA_TERM_DIM  "\033[2m"
#endif
#define PATATA_TERM_RESET "\033[0m"
