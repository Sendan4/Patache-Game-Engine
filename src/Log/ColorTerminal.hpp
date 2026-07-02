#ifndef COLORTERMINAL_HPP
#define COLORTERMINAL_HPP

namespace Patache
{
// Foreground
static constexpr char sTermColorPatache[]{ "\033[38;5;180m" };
static constexpr char sTermColorRed[]{ "\033[38;5;203m" };
static constexpr char sTermColorBlue[]{ "\033[38;5;75m" };
static constexpr char sTermColorGreen[]{ "\033[38;5;118m" };
static constexpr char sTermColorYellow[]{ "\033[38;5;227m" };
static constexpr char sTermColorGray0[]{ "\033[38;5;245m" };
static constexpr char sTermColorGray1[]{ "\033[38;5;250m" };

// Others
#if defined(_WIN64)
static constexpr char sTermDim[]{ "" };
static constexpr char sTermBold[]{ "\033[38;5;15m" };
#else
static constexpr char sTermDim[]{ "\033[1m" };
static constexpr char sTermBold[]{ "\033[2m" };
#endif

static constexpr char sTermReset[]{ "\033[0m" };
}

#endif
