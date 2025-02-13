/// @file colors.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief ANSI colors.

#pragma once

namespace quire
{
/// @brief ANSI escape codes.
namespace ansi
{
/// @brief Foreground colors.
namespace fg
{
// Normal colors.
constexpr const char *black          = "\033[30m"; ///< ANSI foreground color black.
constexpr const char *red            = "\033[31m"; ///< ANSI foreground color red.
constexpr const char *green          = "\033[32m"; ///< ANSI foreground color green.
constexpr const char *yellow         = "\033[33m"; ///< ANSI foreground color yellow.
constexpr const char *blue           = "\033[34m"; ///< ANSI foreground color blue.
constexpr const char *magenta        = "\033[35m"; ///< ANSI foreground color magenta.
constexpr const char *cyan           = "\033[36m"; ///< ANSI foreground color cyan.
constexpr const char *white          = "\033[37m"; ///< ANSI foreground color white.
// Bright colors.
constexpr const char *bright_black   = "\033[30;1m"; ///< ANSI foreground color bright black.
constexpr const char *bright_red     = "\033[31;1m"; ///< ANSI foreground color bright red.
constexpr const char *bright_green   = "\033[32;1m"; ///< ANSI foreground color bright green.
constexpr const char *bright_yellow  = "\033[33;1m"; ///< ANSI foreground color bright yellow.
constexpr const char *bright_blue    = "\033[34;1m"; ///< ANSI foreground color bright blue.
constexpr const char *bright_magenta = "\033[35;1m"; ///< ANSI foreground color bright magenta.
constexpr const char *bright_cyan    = "\033[36;1m"; ///< ANSI foreground color bright cyan.
constexpr const char *bright_white   = "\033[37;1m"; ///< ANSI foreground color bright white.
} // namespace fg
/// @brief Background colors.
namespace bg
{
// Normal colors.
constexpr const char *black          = "\033[40m"; ///< ANSI background color black.
constexpr const char *red            = "\033[41m"; ///< ANSI background color red.
constexpr const char *green          = "\033[42m"; ///< ANSI background color green.
constexpr const char *yellow         = "\033[43m"; ///< ANSI background color yellow.
constexpr const char *blue           = "\033[44m"; ///< ANSI background color blue.
constexpr const char *magenta        = "\033[45m"; ///< ANSI background color magenta.
constexpr const char *cyan           = "\033[46m"; ///< ANSI background color cyan.
constexpr const char *white          = "\033[47m"; ///< ANSI background color white.
// Bright colors.
constexpr const char *bright_black   = "\033[40;1m"; ///< ANSI background color bright black.
constexpr const char *bright_red     = "\033[41;1m"; ///< ANSI background color bright red.
constexpr const char *bright_green   = "\033[42;1m"; ///< ANSI background color bright green.
constexpr const char *bright_yellow  = "\033[43;1m"; ///< ANSI background color bright yellow.
constexpr const char *bright_blue    = "\033[44;1m"; ///< ANSI background color bright blue.
constexpr const char *bright_magenta = "\033[45;1m"; ///< ANSI background color bright magenta.
constexpr const char *bright_cyan    = "\033[46;1m"; ///< ANSI background color bright cyan.
constexpr const char *bright_white   = "\033[47;1m"; ///< ANSI background color bright white.
} // namespace bg
/// @brief Utility escape codes.
namespace util
{
constexpr const char *reset     = "\033[0m"; ///< Reset all styles.
constexpr const char *bold      = "\033[1m"; ///< Turns on the bold text.
constexpr const char *underline = "\033[4m"; ///< Turns on the underline text.
constexpr const char *reverse   = "\033[7m"; ///< Turns on the reverse text.
constexpr const char *clear     = "\033[2J"; ///< Clears entire screen.
constexpr const char *clearline = "\033[2K"; ///< Clears entire line.
constexpr const char *up        = "\033[1A"; ///< Moves the cursor up by one position.
constexpr const char *down      = "\033[1B"; ///< Moves the cursor down by one position.
constexpr const char *right     = "\033[1C"; ///< Moves the cursor right by one position.
constexpr const char *left      = "\033[1D"; ///< Moves the cursor left by one position.
constexpr const char *nextline  = "\033[1E"; ///< Move cursor to beginning of line, 1 line down.
constexpr const char *prevline  = "\033[1F"; ///< Move cursor to beginning of line, 1 line up.
} // namespace util
} // namespace ansi
} // namespace quire
