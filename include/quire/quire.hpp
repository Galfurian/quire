/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Quire logging system header file providing logging functionality with
/// levels, colors, and configurable options.

#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <map>

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
// Normal colors.
extern const char *black;   ///< ANSI background color black.
extern const char *red;     ///< ANSI background color red.
extern const char *green;   ///< ANSI background color green.
extern const char *yellow;  ///< ANSI background color yellow.
extern const char *blue;    ///< ANSI background color blue.
extern const char *magenta; ///< ANSI background color magenta.
extern const char *cyan;    ///< ANSI background color cyan.
extern const char *white;   ///< ANSI background color white.
// Bright colors.
extern const char *bright_black;   ///< ANSI background color bright black.
extern const char *bright_red;     ///< ANSI background color bright red.
extern const char *bright_green;   ///< ANSI background color bright green.
extern const char *bright_yellow;  ///< ANSI background color bright yellow.
extern const char *bright_blue;    ///< ANSI background color bright blue.
extern const char *bright_magenta; ///< ANSI background color bright magenta.
extern const char *bright_cyan;    ///< ANSI background color bright cyan.
extern const char *bright_white;   ///< ANSI background color bright white.
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

/// @brief Defines the default log levels.
enum log_level {
    debug    = 0, ///< Debug level.
    info     = 1, ///< Info level.
    warning  = 2, ///< Warning level.
    error    = 3, ///< Error level.
    critical = 4  ///< Critical level.
};

/// @brief Configuration bitmasks.
enum class option_t {
    header,
    level,
    location,
    date,
    time
};

/// @brief Log level details.
struct log_level_config_t {
    const char *name; ///< Name of the log level (e.g., DEBUG, INFO).
    const char *fg;   ///< Foreground color (ANSI escape code).
    const char *bg;   ///< Background color (ANSI escape code).
};

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
        unsigned _min_level,
        char _separator,
        const std::vector<option_t> &_config = get_default_configuation()) noexcept;

    /// @brief Move constructor.
    /// @param other The logger instance to move from.
    logger_t(logger_t &&other) noexcept;

    /// @brief Destructor for cleanup.
    ~logger_t();

    /// @brief Retrieves the current header.
    std::string get_header() const;

    /// @brief Clears all log levels from the logger.
    /// @details This function removes all entries from the log levels map, including
    /// both default and custom log levels. After this operation, no log levels will
    /// exist, and logging will be effectively disabled until new levels are added.
    void clear_log_levels();

    /// @brief Adds a new log level or updates an existing one.
    /// @param level The log level to add or update.
    /// @param name The name for the log level.
    /// @param fg The foreground color for the log level.
    /// @param bg The background color for the log level.
    /// @return `true` if the log level was updated, `false` if it was newly added.
    logger_t &add_or_update_log_level(
        unsigned level,
        const char *name,
        const char *fg = ansi::fg::white,
        const char *bg = ansi::util::reset);

    /// @brief Retrieves the current log level.
    unsigned get_log_level() const;

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
    logger_t &set_log_level(unsigned _level);

    /// @brief Updates the separator character.
    /// @param _separator New separator character.
    /// @return Reference to the logger instance.
    logger_t &set_separator(char _separator);

    /// @brief Assigns colors for a specific log level.
    /// @param level Log level to set colors for.
    /// @param fg Foreground color (default: white).
    /// @param bg Background color (default: reset).
    /// @return Reference to the logger instance.
    logger_t &set_color(unsigned level, const char *fg, const char *bg);

    /// @brief Enables or disables colored output.
    /// @param enable Whether to enable or disable colored output.
    /// @return Reference to the logger instance.
    logger_t &toggle_color(bool enable);

    /// @brief Configures display options using bitmask settings.
    /// @param _config Header configuration.
    /// @return Reference to the logger instance.
    logger_t &configure(const std::vector<option_t> &_config);

    /// @brief Logs a message with formatting.
    /// @param level Log level.
    /// @param format Format string.
    void log(unsigned level, char const *format, ...);

    /// @brief Logs a message with location information.
    /// @param level Log level.
    /// @param file Source file name.
    /// @param line Source line number.
    /// @param format Format string.
    void log(unsigned level, char const *file, int line, char const *format, ...);

    void print_logger_state() const;

    static inline std::vector<option_t> &get_default_configuation()
    {
        static std::vector<option_t> configuration{ option_t::header, option_t::level, option_t::time, option_t::location };
        return configuration;
    }

    static inline std::vector<option_t> &get_show_all_configuation()
    {
        static std::vector<option_t> configuration{ option_t::header, option_t::level, option_t::date, option_t::time, option_t::location };
        return configuration;
    }

private:
    /// @brief Initializes default log levels and their names.
    void initialize_default_levels();

    /// @brief Helper for formatting messages.
    /// @param format Format string.
    /// @param args Variable arguments.
    void format_message(char const *format, va_list args);

    /// @brief Logs a message by splitting lines and formatting output.
    /// @param level Log level.
    /// @param location Source location.
    /// @param content Message content.
    void write_log(const log_level_config_t &level, const std::string &location, const char *content) const;

    /// @brief Writes formatted log information.
    /// @param level Log level.
    /// @param location Source location.
    /// @param line Message content.
    /// @param length Length of the message.
    void write_log_line(const log_level_config_t &level, const std::string &location, const char *line, std::size_t length) const;

    std::ostream *ostream;                             ///< Output stream for logging.
    std::ostream *fstream;                             ///< File handler for output.
    std::mutex mtx;                                    ///< Mutex for thread safety.
    std::string header;                                ///< Header for each log entry.
    unsigned min_level;                                ///< Minimum log level threshold.
    mutable bool last_log_ended_with_newline;          ///< Tracks if last log ended with newline.
    bool enable_color;                                 ///< Are colors enabled.
    std::vector<option_t> configuration;               ///< Configuration of shown information.
    char separator;                                    ///< Separator character for log components.
    char *buffer;                                      ///< Buffer for formatting log messages.
    std::size_t buffer_length;                         ///< Current buffer size.
    std::map<unsigned, log_level_config_t> log_levels; ///< Custom log levels and their names.
    int log_levels_max_name_length;                    ///< Maximum length of log level names.
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
