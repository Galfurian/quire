/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Quire logging system header file providing logging functionality with
/// levels, colors, and configurable options.

#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include <mutex>

/// @brief Quire source code.
namespace quire
{
/// @brief ANSI escape codes.
namespace ansi
{
/// @brief Foreground colors.
namespace fg
{
// Normal colors.
extern const char *black;   ///< ANSI foreground color black.
extern const char *red;     ///< ANSI foreground color red.
extern const char *green;   ///< ANSI foreground color green.
extern const char *yellow;  ///< ANSI foreground color yellow.
extern const char *blue;    ///< ANSI foreground color blue.
extern const char *magenta; ///< ANSI foreground color magenta.
extern const char *cyan;    ///< ANSI foreground color cyan.
extern const char *white;   ///< ANSI foreground color white.
// Bright colors.
extern const char *bright_black;   ///< ANSI foreground color bright black.
extern const char *bright_red;     ///< ANSI foreground color bright red.
extern const char *bright_green;   ///< ANSI foreground color bright green.
extern const char *bright_yellow;  ///< ANSI foreground color bright yellow.
extern const char *bright_blue;    ///< ANSI foreground color bright blue.
extern const char *bright_magenta; ///< ANSI foreground color bright magenta.
extern const char *bright_cyan;    ///< ANSI foreground color bright cyan.
extern const char *bright_white;   ///< ANSI foreground color bright white.
} // namespace fg
/// @brief Background colors.
namespace bg
{
extern const char *black;   ///< ANSI background color black.
extern const char *red;     ///< ANSI background color red.
extern const char *green;   ///< ANSI background color green.
extern const char *yellow;  ///< ANSI background color yellow.
extern const char *blue;    ///< ANSI background color blue.
extern const char *magenta; ///< ANSI background color magenta.
extern const char *cyan;    ///< ANSI background color cyan.
extern const char *white;   ///< ANSI background color white.
} // namespace bg
/// @brief Utility escape codes.
namespace util
{
extern const char *reset;     ///< Reset all styles.
extern const char *bold;      ///< Turns on the bold text.
extern const char *underline; ///< Turns on the underline text.
extern const char *reverse;   ///< Turns on the reverse text.
extern const char *clear;     ///< Clears entire screen.
extern const char *clearline; ///< Clears entire line.
extern const char *up;        ///< Moves the cursor up by one position.
extern const char *down;      ///< Moves the cursor down by one position.
extern const char *right;     ///< Moves the cursor right by one position.
extern const char *left;      ///< Moves the cursor left by one position.
extern const char *nextline;  ///< Move cursor to beginning of line, 1 line down.
extern const char *prevline;  ///< Move cursor to beginning of line, 1 line up.
} // namespace util
} // namespace ansi

/// @brief Defines the log levels.
enum log_level {
    debug    = 0, ///< Debug level.
    info     = 1, ///< Info level.
    warning  = 2, ///< Warning level.
    error    = 3, ///< Error level.
    critical = 4  ///< Critical level.
};

/// @brief Configuration bitmasks.
enum class option_t {
    level,
    location,
    date,
    time
};

using configuration_t = std::vector<option_t>;

[[maybe_unused]] static configuration_t configuration_show_all{ option_t::level, option_t::location, option_t::date, option_t::location };
[[maybe_unused]] static configuration_t configuration_default{ option_t::level, option_t::date, option_t::location };

/// @brief Logger class for managing log entries with configurations and color options.
class logger_t {
public:
    /// @brief Constructs a logger with specified settings for formatting and filtering log entries.
    /// @param _header Header text included at the start of each log entry.
    /// @param _min_level Minimum log level required for messages to be logged; messages below this level are ignored.
    /// @param _separator Character used to separate different components (e.g., timestamp, level, message) in each log entry.
    /// @param _config Header configuration.
    explicit logger_t(
        std::string _header,
        log_level _min_level,
        char _separator,
        const configuration_t &_config = configuration_default);

    /// @brief Destructor for cleanup.
    ~logger_t();

    /// @brief Retrieves the current header.
    std::string get_header() const;

    /// @brief Retrieves the current log level.
    log_level get_log_level() const;
    /// @brief Resets the log colors to defaults.
    /// @return Reference to the logger instance.
    logger_t &reset_colors();

    /// @brief Sets the file handler for log output.
    /// @param _fstream File handler instance.
    /// @return Reference to the logger instance.
    logger_t &set_file_handler(std::ostream *_fstream);

    /// @brief Sets the output stream for log output.
    /// @param _ostream Output stream.
    /// @return Reference to the logger instance.
    logger_t &set_output_stream(std::ostream *_ostream);

    /// @brief Updates the log header.
    /// @param _header New header string.
    /// @return Reference to the logger instance.
    logger_t &set_header(std::string _header);

    /// @brief Sets the log level threshold.
    /// @param _level Minimum log level.
    /// @return Reference to the logger instance.
    logger_t &set_log_level(log_level _level);

    /// @brief Updates the separator character.
    /// @param _separator New separator character.
    /// @return Reference to the logger instance.
    logger_t &set_separator(char _separator);

    /// @brief Assigns colors for a specific log level.
    /// @param level Log level to set colors for.
    /// @param fg Foreground color (default: white).
    /// @param bg Background color (default: reset).
    /// @return Reference to the logger instance.
    logger_t &set_color(log_level level, const char *fg, const char *bg);

    /// @brief Enables or disables colored output.
    /// @param enable Whether to enable or disable colored output.
    /// @return Reference to the logger instance.
    logger_t &toggle_color(bool enable)
    {
        enable_color = enable;
        return *this;
    }

    /// @brief Configures display options using bitmask settings.
    /// @param _config Header configuration.
    /// @return Reference to the logger instance.
    logger_t &configure(const configuration_t &_config)
    {
        config = _config;
        return *this;
    }

    /// @brief Logs a message with formatting.
    /// @param level Log level.
    /// @param format Format string.
    void log(log_level level, char const *format, ...);

    /// @brief Logs a message with location information.
    /// @param level Log level.
    /// @param file Source file name.
    /// @param line Source line number.
    /// @param format Format string.
    void log(log_level level, char const *file, int line, char const *format, ...);

private:
    /// @brief Helper for formatting messages.
    /// @param format Format string.
    /// @param args Variable arguments.
    void format_message(char const *format, va_list args);

    /// @brief Logs a message by splitting lines and formatting output.
    /// @param level Log level.
    /// @param location Source location.
    /// @param content Message content.
    void write_log(log_level level, const std::string &location, const char *content) const;

    /// @brief Writes formatted log information.
    /// @param level Log level.
    /// @param location Source location.
    /// @param line Message content.
    /// @param length Length of the message.
    void write_log_line(log_level level, const std::string &location, const char *line, std::size_t length) const;

    std::ostream *ostream;                    ///< Output stream for logging.
    std::ostream *fstream;                    ///< File handler for output.
    std::mutex mtx;                           ///< Mutex for thread safety.
    std::string header;                       ///< Header for each log entry.
    log_level min_level;                      ///< Minimum log level threshold.
    mutable bool last_log_ended_with_newline; ///< Tracks if last log ended with newline.
    bool enable_color;                        ///< Are colors enabled.
    configuration_t config;                   ///< Configuration of shown information.
    char separator;                           ///< Separator character for log components.
    char *buffer;                             ///< Buffer for formatting log messages.
    std::size_t buffer_length;                ///< Current buffer size.
    const char *fg_colors[5];                 ///< Foreground colors for each log level.
    const char *bg_colors[5];                 ///< Background colors for each log level.
};

} // namespace quire

/// @brief Logs the message, with the given level.
#define qlog(logger, level, ...) logger.log(level, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the debug message.
#define qdebug(logger, ...) qlog(logger, quire::debug, __VA_ARGS__)

/// @brief Logs the info message.
#define qinfo(logger, ...) qlog(logger, quire::info, __VA_ARGS__)

/// @brief Logs the warning message.
#define qwarning(logger, ...) qlog(logger, quire::warning, __VA_ARGS__)

/// @brief Logs the error message.
#define qerror(logger, ...) qlog(logger, quire::error, __VA_ARGS__)

/// @brief Logs the critical message.
#define qcritical(logger, ...) qlog(logger, quire::critical, __VA_ARGS__)
