/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief The quire logger source dode.

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

/// @brief Handles output files.
class file_handler_t {
private:
    /// @brief An handler for the file.
    FILE *stream;

public:
    /// @brief Construct the handler and opens the file.
    /// @param filename the filename.
    /// @param mode the mode with which we open it.
    file_handler_t(const char *filename, const char *mode);

    /// @brief Destroy the handler and closes the file.
    ~file_handler_t();

    /// @brief Writes the buffer on file, if the handler is valid.
    /// @param buffer the buffer to write.
    /// @return how much we wrote, 0 if we write nothing.
    std::size_t write(const std::string &buffer);
};

/// @brief The logger class.
class logger_t {
public:
    /// @brief Construct a new logger.
    /// @param _header the header shown every time the log is printed.
    /// @param _min_level the minimal level we will print for this logger.
    /// @param _separator the semparator used for dividing all the log information.
    explicit logger_t(std::string _header, log_level _min_level, char _separator);

    ~logger_t();

    /// @brief Retuns the header.
    std::string get_header() const;

    /// @brief Resets the color to the default ones.
    void reset_colors();

    /// @brief Sets the file handler.
    void set_file_handler(std::shared_ptr<file_handler_t> _fhandler);

    /// @brief Sets the output stream.
    void set_output_stream(std::ostream *_stream);

    /// @brief Sets the header.
    void set_header(std::string _header);

    /// @brief Sets the log level.
    void set_log_level(log_level _level);

    /// @brief Sets the separator.
    void set_separator(char _separator);

    /// @brief Sets the color for a given log level.
    /// @param level the level we want to customize.
    /// @param fg the foreground color we want to use (default: quire::ansi::fg::white).
    /// @param bg the background color we want to use (default: quire::ansi::util::reset).
    void set_color(log_level level, const char *fg, const char *bg);

    /// @brief Turns on/off the level.
    void configure(int _config);

    /// @brief Turns on/off the level.
    void toggle_level(bool enable);

    /// @brief Turns on/off the color.
    void toggle_color(bool enable);

    /// @brief Turns on/off the date.
    void toggle_date(bool enable);

    /// @brief Turns on/off the time.
    void toggle_time(bool enable);

    /// @brief Turns on/off the location inside the file.
    void toggle_location(bool enable);

    /// @brief Perform the logging.
    void log(log_level level, char const *format, ...);

    /// @brief Perform the logging.
    void log(log_level level, char const *file, int line, char const *format, ...);

private:
    /// @brief Helper function to handle formatting and buffer allocation.
    /// @param format The format string for the message.
    /// @param args The variable argument list for formatting.
    void format_message(char const *format, va_list args);

    /// @brief Logs information with specified log level and location to the stream or file.
    /// @param level The log level to log (e.g., info, warning, error).
    /// @param location The source location where the log is generated.
    /// @param line The buffer containing the log message to be logged.
    void do_log(log_level level, const std::string &location, const char *line) const;

    /// @brief A pointer to the file handler.
    std::shared_ptr<file_handler_t> fhandler;
    /// @brief A pointer to the output stream.
    std::ostream *stream;
    /// @brief A mutex to protect the use of the logger.
    std::mutex mtx;
    /// @brief A header which is pre-pended to each logger line.
    std::string header;
    /// @brief The minimum level we are going to log.
    log_level min_level;
    // Class member to track if the last log ended with a newline
    mutable bool last_log_ended_with_newline;
    /// @brief Internal configuration of the logger.
    int config;
    /// @brief The character we use to separate elements of the log.
    char separator;
    /// @brief Internal buffer for handling output formatting.
    char *buffer;
    /// @brief The current size of the buffer.
    std::size_t buffer_length;
    /// @brief Keeps track of the association between log level and foreground color.
    const char *fg_colors[5];
    /// @brief Keeps track of the association between log level and background color.
    const char *bg_colors[5];
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
