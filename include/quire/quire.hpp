/// @file quire.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief The quire logger source dode.

#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

namespace quire
{
namespace ansi
{
namespace fg
{
const char *black   = "\u001b[30m"; ///< ANSI foreground color black.
const char *red     = "\u001b[31m"; ///< ANSI foreground color red.
const char *green   = "\u001b[32m"; ///< ANSI foreground color green.
const char *yellow  = "\u001b[33m"; ///< ANSI foreground color yellow.
const char *blue    = "\u001b[34m"; ///< ANSI foreground color blue.
const char *magenta = "\u001b[35m"; ///< ANSI foreground color magenta.
const char *cyan    = "\u001b[36m"; ///< ANSI foreground color cyan.
const char *white   = "\u001b[37m"; ///< ANSI foreground color white.

const char *bright_black   = "\u001b[30;1m"; ///< ANSI foreground color bright black.
const char *bright_red     = "\u001b[31;1m"; ///< ANSI foreground color bright red.
const char *bright_green   = "\u001b[32;1m"; ///< ANSI foreground color bright green.
const char *bright_yellow  = "\u001b[33;1m"; ///< ANSI foreground color bright yellow.
const char *bright_blue    = "\u001b[34;1m"; ///< ANSI foreground color bright blue.
const char *bright_magenta = "\u001b[35;1m"; ///< ANSI foreground color bright magenta.
const char *bright_cyan    = "\u001b[36;1m"; ///< ANSI foreground color bright cyan.
const char *bright_white   = "\u001b[37;1m"; ///< ANSI foreground color bright white.
} // namespace fg

namespace bg
{

const char *black   = "\u001b[40m"; ///< ANSI background color black.
const char *red     = "\u001b[41m"; ///< ANSI background color red.
const char *green   = "\u001b[42m"; ///< ANSI background color green.
const char *yellow  = "\u001b[43m"; ///< ANSI background color yellow.
const char *blue    = "\u001b[44m"; ///< ANSI background color blue.
const char *magenta = "\u001b[45m"; ///< ANSI background color magenta.
const char *cyan    = "\u001b[46m"; ///< ANSI background color cyan.
const char *white   = "\u001b[47m"; ///< ANSI background color white.
} // namespace bg

namespace util
{
const char *reset     = "\u001b[0m"; ///< Reset all styles.
const char *bold      = "\u001b[1m"; ///< Turns on the bold text.
const char *underline = "\u001b[4m"; ///< Turns on the underline text.
const char *reverse   = "\u001b[7m"; ///< Turns on the reverse text.
const char *clear     = "\u001b[2J"; ///< Clears entire screen.
const char *clearline = "\u001b[2K"; ///< Clears entire line.
const char *up        = "\u001b[1A"; ///< Moves the cursor up by one position.
const char *down      = "\u001b[1B"; ///< Moves the cursor down by one position.
const char *right     = "\u001b[1C"; ///< Moves the cursor right by one position.
const char *left      = "\u001b[1D"; ///< Moves the cursor left by one position.
const char *nextline  = "\u001b[1E"; ///< Move cursor to beginning of line, 1 line down.
const char *prevline  = "\u001b[1F"; ///< Move cursor to beginning of line, 1 line up.
} // namespace util
} // namespace ansi

/// @brief Get the current date.
inline std::string get_date()
{
    char buffer[32];
    time_t now = time(nullptr);
    strftime(buffer, 32, "%d/%m/%y", localtime(&now));
    return std::string(buffer);
}

/// @brief Get the current time.
inline std::string get_time()
{
    char buffer[32];
    time_t now = time(nullptr);
    strftime(buffer, 32, "%H:%M", localtime(&now));
    return std::string(buffer);
}

/// @brief Defines the log levels.
enum class log_level {
    debug    = 10, ///< Debug level.
    info     = 20, ///< Info level.
    warning  = 30, ///< Warning level.
    error    = 40, ///< Error level.
    critical = 50, ///< Critical level.
};

/// @brief Transforms the log level to string.
/// @param level the log level.
/// @return the corresponding string.
inline const char *log_level_to_string(log_level level)
{
    if (level == log_level::debug) {
        return "debug";
    }
    if (level == log_level::info) {
        return "info";
    }
    if (level == log_level::warning) {
        return "warning";
    }
    if (level == log_level::error) {
        return "error";
    }
    return "critical";
}

/// @brief Handles output files.
class file_handler_t {
private:
    /// @brief An handler for the file.
    FILE *stream;

public:
    /// @brief Construct the handler and opens the file.
    /// @param filename the filename.
    /// @param mode the mode with which we open it.
    file_handler_t(const char *filename, const char *mode)
        : stream(fopen(filename, mode))
    {
        // Nothing to do.
    }

    /// @brief Destroy the handler and closes the file.
    ~file_handler_t()
    {
        fclose(stream);
    }

    /// @brief Writes the buffer on file, if the handler is valid.
    /// @param buffer the buffer to write.
    /// @return how much we wrote, 0 if we write nothing.
    std::size_t write(const std::string &buffer)
    {
        if (stream) {
            return fwrite(buffer.c_str(), buffer.size(), 1, stream);
        }
        return 0;
    }
};

/// @brief The logger class.
class logger_t {
private:
    /// @brief A pointer to the file handler.
    std::unique_ptr<file_handler_t> fhandler;
    /// @brief A pointer to the output stream.
    std::ostream *stream;
    /// @brief A mutex to protect the use of the logger.
    std::mutex mtx;
    /// @brief A header which is pre-pended to each logger line.
    std::string header;
    /// @brief The minimum level we are going to log.
    log_level min_level;
    /// @brief Internal configuration of the logger.
    int config;
    /// @brief The character we use to separate elements of the log.
    char separator;

    /// @brief Configuration bitmasks.
    enum configuration {
        none          = 0u,         ///< No bit.
        show_level    = (1u << 0u), ///< We show the log level.
        show_date     = (1u << 1u), ///< We show the data.
        show_time     = (1u << 2u), ///< We show the time.
        show_colored  = (1u << 3u), ///< We show a colored log.
        show_location = (1u << 4u), ///< We show the location inside the file.
    };

public:
    /// @brief Construct a new logger.
    explicit logger_t(std::string _header, log_level _min_level, char _separator)
        : fhandler(),
          stream(&std::cout),
          mtx(),
          header(_header),
          min_level(_min_level),
          config(0),
          separator(_separator)
    {
        // Nothing to do.
    }

    /// @brief Sets the file handler.
    void set_file_handler(std::unique_ptr<file_handler_t> &_fhandler)
    {
        fhandler = std::move(_fhandler);
    }

    /// @brief Sets the output stream.
    void set_output_stream(std::ostream *_stream)
    {
        stream = _stream;
    }

    /// @brief Sets the header.
    void set_header(std::string _header)
    {
        header = _header;
    }

    /// @brief Sets the log level.
    void set_log_level(log_level _level)
    {
        min_level = _level;
    }

    /// @brief Sets the separator.
    void set_separator(char _separator)
    {
        separator = _separator;
    }

    /// @brief Turns on/off the level.
    void toggle_level(bool enable)
    {
        (enable) ? (config |= configuration::show_level) : (config &= ~configuration::show_level);
    }

    /// @brief Turns on/off the color.
    void toggle_color(bool enable)
    {
        (enable) ? (config |= configuration::show_colored) : (config &= ~configuration::show_colored);
    }

    /// @brief Turns on/off the date.
    void toggle_date(bool enable)
    {
        (enable) ? (config |= configuration::show_date) : (config &= ~configuration::show_date);
    }

    /// @brief Turns on/off the time.
    void toggle_time(bool enable)
    {
        (enable) ? (config |= configuration::show_time) : (config &= ~configuration::show_time);
    }

    /// @brief Turns on/off the location inside the file.
    void toggle_location(bool enable)
    {
        (enable) ? (config |= configuration::show_location) : (config &= ~configuration::show_location);
    }

    /// @brief Perform the logging.
    void log(log_level level, char const *file, int line, char const *format, ...)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (level < min_level) {
            return;
        }

        // == FLAGS ==========================================================

        bool show_level    = config & configuration::show_level;
        bool show_date     = config & configuration::show_date;
        bool show_time     = config & configuration::show_time;
        bool show_colored  = config & configuration::show_colored;
        bool show_location = config & configuration::show_location;

        // == BUILD LOG =======================================================

        va_list length_args;
        va_start(length_args, format);
        va_list result_args;
        va_copy(result_args, length_args);
        const auto length = std::vsnprintf(nullptr, 0U, format, length_args);
        std::string result(static_cast<std::string::size_type>(length), '\0');
        std::vsprintf(std::data(result), format, result_args);
        va_end(result_args);
        va_end(length_args);

        // == STREAM ==========================================================
        std::stringstream ss;

        // == LOG =============================================================
        if (!header.empty()) {
            ss << header << " " << separator << " ";
        }
        if (show_level) {
            ss << std::left << std::setw(9) << quire::log_level_to_string(level) << separator << " ";
        }
        if (show_date) {
            ss << quire::get_date() << " ";
            if (!show_time) {
                ss << separator << " ";
            }
        }
        if (show_time) {
            ss << quire::get_time() << " " << separator << " ";
        }
        if (show_location) {
            ss << logger_t::assemble_location(file, line) << " " << separator << " ";
        }
        ss << result << "\n";

        // == COLOR (OFF) =====================================================

        // == WRITE STREAM ====================================================
        if (fhandler) {
            fhandler->write(ss.str());
        }

        // == COLOR (ON) ======================================================

        if (stream) {
            if (show_colored) {
                if (level == log_level::critical) {
                    (*stream) << ansi::fg::bright_red;
                } else if (level == log_level::error) {
                    (*stream) << ansi::fg::red;
                } else if (level == log_level::warning) {
                    (*stream) << ansi::fg::bright_yellow;
                } else if (level == log_level::info) {
                    (*stream) << ansi::fg::bright_white;
                } else {
                    (*stream) << ansi::fg::cyan;
                }
            }
            (*stream) << ss.str();
            if (show_colored) {
                (*stream) << ansi::util::reset;
            }
        }
    }

private:
    static inline std::string assemble_location(char const *file, int line)
    {
        return std::string(std::filesystem::path(file).filename()) + ":" + std::to_string(line);
    }
};

/// @brief Creates a new logger.
inline std::unique_ptr<logger_t> get_logger()
{
    return std::make_unique<logger_t>("", log_level::info, '|');
}

/// @brief Creates a new logger.
inline std::unique_ptr<logger_t> get_logger(std::string _header, log_level _min_level, char _separator)
{
    return std::make_unique<logger_t>(_header, _min_level, _separator);
}

/// @brief Creates a new file handler.
inline std::unique_ptr<file_handler_t> get_file_handler(const char *filename, const char *mode)
{
    return std::make_unique<file_handler_t>(filename, mode);
}

} // namespace quire

/// @brief Logs the message, with the given level.
#define qlog(LOGGER, LEVEL, ...) (LOGGER->log(LEVEL, __FILE__, __LINE__, __VA_ARGS__))

/// @brief Logs the debug message.
#define qdebug(LOGGER, ...) qlog(LOGGER, quire::log_level::debug, __VA_ARGS__)

/// @brief Logs the info message.
#define qinfo(LOGGER, ...) qlog(LOGGER, quire::log_level::info, __VA_ARGS__)

/// @brief Logs the warning message.
#define qwarning(LOGGER, ...) qlog(LOGGER, quire::log_level::warning, __VA_ARGS__)

/// @brief Logs the error message.
#define qerror(LOGGER, ...) qlog(LOGGER, quire::log_level::error, __VA_ARGS__)

/// @brief Logs the critical message.
#define qcritical(LOGGER, ...) qlog(LOGGER, quire::log_level::critical, __VA_ARGS__)
