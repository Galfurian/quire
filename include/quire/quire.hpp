/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Quire logging system header file providing logging functionality with
/// levels, colors, and configurable options.

#pragma once

#include <fstream>
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
enum configuration {
    show_nothing  = 0u,                                                               ///< No bit.
    show_level    = (1u << 0u),                                                       ///< We show the log level.
    show_date     = (1u << 1u),                                                       ///< We show the data.
    show_time     = (1u << 2u),                                                       ///< We show the time.
    show_colored  = (1u << 3u),                                                       ///< We show a colored log.
    show_location = (1u << 4u),                                                       ///< We show the location inside the file.
    show_all      = show_level | show_date | show_time | show_colored | show_location ///< We show everything.
};

/// @brief Manages file output for logs.
class file_handler_t {
private:
    FILE *stream; ///< File pointer for log output.

public:
    /// @brief Opens a file for logging.
    /// @param filename Name of the file.
    /// @param mode Mode in which the file is opened.
    file_handler_t(const char *filename, const char *mode);

    /// @brief Closes the file upon destruction.
    ~file_handler_t();

    /// @brief Writes the provided buffer to the file.
    /// @param buffer Content to be written.
    /// @return Number of bytes written, 0 if failed.
    std::size_t write(const std::string &buffer);
};

/// @brief Logger class for managing log entries with configurations and color options.
class logger_t {
public:
    /// @brief Constructs a logger with specified settings.
    /// @param _header Header for each log entry.
    /// @param _min_level Minimum level to be logged.
    /// @param _separator Character separating log components.
    explicit logger_t(std::string _header, log_level _min_level, char _separator);

    /// @brief Destructor for cleanup.
    ~logger_t();

    /// @brief Retrieves the current header.
    std::string get_header() const;

    /// @brief Retrieves the current log level.
    log_level get_log_level() const;

    /// @brief Resets the log colors to defaults.
    void reset_colors();

    /// @brief Sets the file handler for log output.
    /// @param _fhandler File handler instance.
    void set_file_handler(std::shared_ptr<file_handler_t> _fhandler);

    /// @brief Sets the output stream for log output.
    /// @param _stream Output stream.
    void set_output_stream(std::ostream *_stream);

    /// @brief Updates the log header.
    /// @param _header New header string.
    void set_header(std::string _header);

    /// @brief Sets the log level threshold.
    /// @param _level Minimum log level.
    void set_log_level(log_level _level);

    /// @brief Updates the separator character.
    /// @param _separator New separator character.
    void set_separator(char _separator);

    /// @brief Assigns colors for a specific log level.
    /// @param level Log level to set colors for.
    /// @param fg Foreground color (default: white).
    /// @param bg Background color (default: reset).
    void set_color(log_level level, const char *fg, const char *bg);

    /// @brief Configures display options using bitmask settings.
    /// @param _config Bitmask configuration.
    void configure(int _config);

    /// @brief Enables or disables log level display.
    void toggle_level(bool enable);

    /// @brief Enables or disables colored output.
    void toggle_color(bool enable);

    /// @brief Enables or disables date display.
    void toggle_date(bool enable);

    /// @brief Enables or disables time display.
    void toggle_time(bool enable);

    /// @brief Enables or disables file location display.
    void toggle_location(bool enable);

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

    std::shared_ptr<file_handler_t> fhandler; ///< File handler for output.
    std::ostream *stream;                     ///< Output stream for logging.
    std::mutex mtx;                           ///< Mutex for thread safety.
    std::string header;                       ///< Header for each log entry.
    log_level min_level;                      ///< Minimum log level threshold.
    mutable bool last_log_ended_with_newline; ///< Tracks if last log ended with newline.
    int config;                               ///< Configuration bitmask.
    char separator;                           ///< Separator character for log components.
    char *buffer;                             ///< Buffer for formatting log messages.
    std::size_t buffer_length;                ///< Current buffer size.
    const char *fg_colors[5];                 ///< Foreground colors for each log level.
    const char *bg_colors[5];                 ///< Background colors for each log level.
};

} // namespace quire

/// @brief Logs the message, with the given level.
#define qlog(logger, level, ...) logger->log(level, __FILE__, __LINE__, __VA_ARGS__)

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
