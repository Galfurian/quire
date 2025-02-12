/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Quire logging system header file providing logging functionality with
/// levels, colors, and configurable options.

#pragma once

#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "colors.hpp"

enum : unsigned char {
    QUIRE_MAJOR_VERSION = 1, ///< Major version of the library.
    QUIRE_MINOR_VERSION = 1, ///< Minor version of the library.
    QUIRE_MICRO_VERSION = 1  ///< Micro version of the library.
};

/// @brief Quire source code.
namespace quire
{

/// @brief Defines the default log levels.
enum log_level : unsigned char {
    debug    = 0, ///< Debug level.
    info     = 1, ///< Info level.
    warning  = 2, ///< Warning level.
    error    = 3, ///< Error level.
    critical = 4, ///< Critical level.
};

/// @brief Configuration bitmasks.
enum class option_t : unsigned char {
    header,   ///< Include a header in the output.
    level,    ///< Include the logging level (e.g., INFO, DEBUG).
    location, ///< Include the source file location in the output.
    date,     ///< Include the current date in the output.
    time      ///< Include the current time in the output.
};

/// @brief Log level details.
struct log_level_config_t {
    const char *name; ///< Name of the log level (e.g., DEBUG, INFO).
    const char *fg;   ///< Foreground color (ANSI escape code).
    const char *bg;   ///< Background color (ANSI escape code).
};

/// @brief Logger class for managing log entries with configurations and color options.
class logger_t
{
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

    /// @brief Copy constructor.
    /// @param other The logger instance to copy from.
    logger_t(const logger_t &other);

    /// @brief Copy assignment operator.
    /// @param other The logger instance to copy from.
    /// @return Reference to the logger instance.
    auto operator=(const logger_t &other) -> logger_t &;

    /// @brief Move assignment operator.
    /// @param other The logger instance to move from.
    /// @return Reference to the logger instance.
    auto operator=(logger_t &&other) noexcept -> logger_t &;

    /// @brief Destructor for cleanup.
    ~logger_t() = default;

    /// @brief Retrieves the current header.
    /// @return The current header.
    auto get_header() const -> std::string;

    /// @brief Clears all log levels from the logger.
    /// @details This function removes all entries from the log levels map, including
    /// both default and custom log levels. After this operation, no log levels will
    /// exist, and logging will be effectively disabled until new levels are added.
    void clear_log_levels();
    
    /// @brief Resets to the default log levels and their names.
    void reset_log_levels();

    /// @brief Adds a new log level or updates an existing one.
    /// @param level The log level to add or update.
    /// @param name The name for the log level.
    /// @param fg The foreground color for the log level.
    /// @param bg The background color for the log level.
    /// @return `true` if the log level was updated, `false` if it was newly added.
    auto add_or_update_log_level(
        unsigned level,
        const char *name,
        const char *fg = ansi::fg::white,
        const char *bg = ansi::util::reset) -> logger_t &;

    /// @brief Retrieves the current log level.
    /// @return The current log level.
    auto get_log_level() const -> unsigned;

    /// @brief Sets the file handler for log output.
    /// @param _fstream File handler instance.
    /// @return Reference to the logger instance.
    auto set_file_handler(std::ostream *_fstream) -> logger_t &;

    /// @brief Sets the output stream for log output.
    /// @param _ostream Output stream.
    /// @return Reference to the logger instance.
    auto set_output_stream(std::ostream *_ostream) -> logger_t &;

    /// @brief Updates the log header.
    /// @param _header New header string.
    /// @return Reference to the logger instance.
    auto set_header(std::string _header) -> logger_t &;

    /// @brief Sets the log level threshold.
    /// @param _level Minimum log level.
    /// @return Reference to the logger instance.
    auto set_log_level(unsigned _level) -> logger_t &;

    /// @brief Updates the separator character.
    /// @param _separator New separator character.
    /// @return Reference to the logger instance.
    auto set_separator(char _separator) -> logger_t &;

    /// @brief Assigns colors for a specific log level.
    /// @param level Log level to set colors for.
    /// @param fg Foreground color (default: white).
    /// @param bg Background color (default: reset).
    /// @return Reference to the logger instance.
    auto set_color(unsigned level, const char *fg, const char *bg) -> logger_t &;

    /// @brief Enables or disables colored output.
    /// @param enable Whether to enable or disable colored output.
    /// @return Reference to the logger instance.
    auto toggle_color(bool enable) -> logger_t &;

    /// @brief Configures display options using bitmask settings.
    /// @param _config Header configuration.
    /// @return Reference to the logger instance.
    auto configure(const std::vector<option_t> &_config) -> logger_t &;

    /// @brief Logs a message with formatting.
    /// @param level Log level.
    /// @param format Format string.
    /// @param args the argumeents.
    template <typename... Args>
    void log(unsigned level, const char *format, Args &&...args)
    {
        // Ensure thread safety by locking the mutex.
        std::lock_guard<std::mutex> lock(mtx);
        // Check if the log level exists in the map.
        auto it = log_levels.find(level);
        // Check if we should log.
        if ((it != log_levels.end()) && (level >= min_level)) {
            // Format and log the message using variadic arguments.
            this->format_message(format, std::forward<Args>(args)...);
            this->write_log(it->second);
        }
    }

    /// @brief Logs a message with location information.
    /// @param level Log level.
    /// @param file Source file name.
    /// @param line Source line number.
    /// @param format Format string.
    /// @param args the argumeents.
    template <typename... Args>
    void log(unsigned level, const char *file, int line, const char *format, Args &&...args)
    {
        // Ensure thread safety by locking the mutex.
        std::lock_guard<std::mutex> lock(mtx);
        // Check if the log level exists in the map.
        auto it = log_levels.find(level);
        // Check if we should log.
        if ((it != log_levels.end()) && (level >= min_level)) {
            // Format and log the message using variadic arguments.
            this->format_message(format, std::forward<Args>(args)...);
            // Pass the level, location, and buffer to do_log.
            this->write_log(it->second, this->assemble_location(file, line));
        }
    }

    /// @brief Prints on output the overall state of the logger.
    void print_logger_state() const;

    /// @brief Returns the default configuration for the logger.
    /// @return the default configuration for the logger.
    static auto get_default_configuation() -> std::vector<option_t> &
    {
        static std::vector<option_t> configuration{
            option_t::header,
            option_t::level,
            option_t::time,
            option_t::location,
        };
        return configuration;
    }

    /// @brief Returns a configuration that shows all information.
    /// @return the configuration that shows all information.
    static auto get_show_all_configuation() -> std::vector<option_t> &
    {
        static std::vector<option_t> configuration{
            option_t::header, option_t::level, option_t::date, option_t::time, option_t::location,
        };
        return configuration;
    }

private:
    /// @brief Combines filename and line number.
    /// @param file Source file name.
    /// @param line Source line number.
    static auto assemble_location(const std::string &file, int line) -> std::string;

    /// @brief Helper for formatting messages.
    /// @param format Format string.
    /// @param args Variable arguments.
    template <typename... Args>
    void format_message(const char *format, Args &&...args)
    {
        if ((format == nullptr) || (std::strlen(format) == 0)) {
            // Clear buffer for empty format.
            buffer.clear();
            return;
        }
        // Ensure buffer has an initial size to prevent undefined behavior.
        if (buffer.empty()) {
            // Start with an initial buffer size.
            buffer.resize(256);
        }
        while (true) {
            int needed_length = 0;
            // Case when there are format arguments.
            if constexpr (sizeof...(args) > 0) {
                needed_length = std::snprintf(buffer.data(), buffer.size(), format, std::forward<Args>(args)...);
            }
            // Case when there are no format arguments.
            else {
                needed_length = std::snprintf(buffer.data(), buffer.size(), "%s", format);
            }
            // Check if snprintf succeeded and if the message fits in the buffer.
            if (needed_length >= 0 && static_cast<size_t>(needed_length) < buffer.size()) {
                // Resize to actual message size.
                buffer.resize(static_cast<size_t>(needed_length));
                // Exit the loop.
                break;
            }
            // Formatting error occurred.
            if (needed_length < 0) {
                buffer.clear();
                break;
            }
            // Buffer was too small; resize and retry.
            buffer.resize(static_cast<size_t>(needed_length) + 1);
        }
    }

    /// @brief Logs a message by splitting lines and formatting output.
    /// @param level Log level.
    /// @param location Source location.
    /// @param content Message content.
    void write_log(const log_level_config_t &level, const std::string &location = std::string()) const;

    /// @brief Writes formatted log information.
    /// @param level Log level.
    /// @param location Source location.
    /// @param line Message content.
    /// @param length Length of the message.
    void write_log_line(
        const log_level_config_t &level,
        const std::string &location,
        const std::string &line,
        std::size_t length) const;

    std::ostream *ostream;                             ///< Output stream for logging.
    std::ostream *fstream;                             ///< File handler for output.
    std::mutex mtx;                                    ///< Mutex for thread safety.
    std::string header;                                ///< Header for each log entry.
    unsigned min_level;                                ///< Minimum log level threshold.
    mutable bool last_log_ended_with_newline;          ///< Tracks if last log ended with newline.
    bool enable_color;                                 ///< Are colors enabled.
    std::vector<option_t> configuration;               ///< Configuration of shown information.
    char separator;                                    ///< Separator character for log components.
    std::string buffer;                                ///< Buffer for formatting log messages.
    std::map<unsigned, log_level_config_t> log_levels; ///< Custom log levels and their names.
    int log_levels_max_name_length;                    ///< Maximum length of log level names.
};

} // namespace quire

/// @brief Logs the message, with the given level.
#define qlog(logger, level, ...) logger.log(level, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the debug message.
#define qdebug(logger, ...) logger.log(quire::debug, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the info message.
#define qinfo(logger, ...) logger.log(quire::info, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the warning message.
#define qwarning(logger, ...) logger.log(quire::warning, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the error message.
#define qerror(logger, ...) logger.log(quire::error, __FILE__, __LINE__, __VA_ARGS__)

/// @brief Logs the critical message.
#define qcritical(logger, ...) logger.log(quire::critical, __FILE__, __LINE__, __VA_ARGS__)
