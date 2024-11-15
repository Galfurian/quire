/// @file quire.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/quire.hpp"

#include <exception>
#include <stdexcept>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>

const char *quire::ansi::fg::black   = "\33[30m";
const char *quire::ansi::fg::red     = "\33[31m";
const char *quire::ansi::fg::green   = "\33[32m";
const char *quire::ansi::fg::yellow  = "\33[33m";
const char *quire::ansi::fg::blue    = "\33[34m";
const char *quire::ansi::fg::magenta = "\33[35m";
const char *quire::ansi::fg::cyan    = "\33[36m";
const char *quire::ansi::fg::white   = "\33[37m";

const char *quire::ansi::fg::bright_black   = "\33[30;1m";
const char *quire::ansi::fg::bright_red     = "\33[31;1m";
const char *quire::ansi::fg::bright_green   = "\33[32;1m";
const char *quire::ansi::fg::bright_yellow  = "\33[33;1m";
const char *quire::ansi::fg::bright_blue    = "\33[34;1m";
const char *quire::ansi::fg::bright_magenta = "\33[35;1m";
const char *quire::ansi::fg::bright_cyan    = "\33[36;1m";
const char *quire::ansi::fg::bright_white   = "\33[37;1m";

const char *quire::ansi::bg::black   = "\33[40m";
const char *quire::ansi::bg::red     = "\33[41m";
const char *quire::ansi::bg::green   = "\33[42m";
const char *quire::ansi::bg::yellow  = "\33[43m";
const char *quire::ansi::bg::blue    = "\33[44m";
const char *quire::ansi::bg::magenta = "\33[45m";
const char *quire::ansi::bg::cyan    = "\33[46m";
const char *quire::ansi::bg::white   = "\33[47m";

const char *quire::ansi::util::reset     = "\33[0m";
const char *quire::ansi::util::bold      = "\33[1m";
const char *quire::ansi::util::underline = "\33[4m";
const char *quire::ansi::util::reverse   = "\33[7m";
const char *quire::ansi::util::clear     = "\33[2J";
const char *quire::ansi::util::clearline = "\33[2K";
const char *quire::ansi::util::up        = "\33[1A";
const char *quire::ansi::util::down      = "\33[1B";
const char *quire::ansi::util::right     = "\33[1C";
const char *quire::ansi::util::left      = "\33[1D";
const char *quire::ansi::util::nextline  = "\33[1E";
const char *quire::ansi::util::prevline  = "\33[1F";

namespace quire
{

/// @brief Get the current date.
static inline std::string __get_date()
{
    char buffer[32];
    time_t now = time(nullptr);
    strftime(buffer, 32, "%d/%m/%y", localtime(&now));
    return std::string(buffer);
}

/// @brief Get the current time.
static inline std::string __get_time()
{
    char buffer[32];
    time_t now = time(nullptr);
    strftime(buffer, 32, "%H:%M", localtime(&now));
    return std::string(buffer);
}

/// @brief Transforms the log level to string.
/// @param level the log level.
/// @return the corresponding string.
static inline const char *__log_level_to_string(log_level level)
{
    if (level == debug) {
        return "debug   ";
    }
    if (level == info) {
        return "info    ";
    }
    if (level == warning) {
        return "warning ";
    }
    if (level == error) {
        return "error   ";
    }
    return "critical";
}

/// @brief Combines filename and line number.
static inline std::string __assemble_location(const std::string &file, int line)
{
    std::stringstream ss;
    ss << line;
    return file.substr(file.find_last_of("/\\") + 1) + ":" + ss.str();
}

logger_t::logger_t(std::string _header, log_level _min_level, char _separator, const configuration_t &_config) noexcept
    : ostream(&std::cout),
      fstream(NULL),
      mtx(),
      header(_header),
      min_level(_min_level),
      last_log_ended_with_newline(true),
      enable_color(true),
      config(_config),
      separator(_separator),
      buffer(nullptr),
      buffer_length(0),
      fg_colors(),
      bg_colors()
{
    // Default foreground colors.
    fg_colors[debug]    = ansi::fg::cyan;
    fg_colors[info]     = ansi::fg::bright_white;
    fg_colors[warning]  = ansi::fg::bright_yellow;
    fg_colors[error]    = ansi::fg::red;
    fg_colors[critical] = ansi::fg::bright_red;

    // By default we do not have background colors.
    bg_colors[debug]    = quire::ansi::util::reset;
    bg_colors[info]     = quire::ansi::util::reset;
    bg_colors[warning]  = quire::ansi::util::reset;
    bg_colors[error]    = quire::ansi::util::reset;
    bg_colors[critical] = quire::ansi::util::reset;
}

logger_t::logger_t(logger_t &&other) noexcept
    : ostream(other.ostream),
      fstream(other.fstream),
      header(std::move(other.header)),
      min_level(other.min_level),
      last_log_ended_with_newline(other.last_log_ended_with_newline),
      enable_color(other.enable_color),
      config(std::move(other.config)),
      separator(other.separator),
      buffer(other.buffer),
      buffer_length(other.buffer_length)
{
    // Move the fg_colors and bg_colors arrays
    std::copy(std::begin(other.fg_colors), std::end(other.fg_colors), fg_colors);
    std::copy(std::begin(other.bg_colors), std::end(other.bg_colors), bg_colors);

    // Nullify moved-from resources in `other`.
    other.ostream       = nullptr;
    other.fstream       = nullptr;
    other.buffer        = nullptr;
    other.buffer_length = 0;
}

void logger_t::print_logger_state() const
{
    std::cout << "ostream       : " << (ostream ? "valid" : "null") << '\n';
    std::cout << "fstream       : " << (fstream ? "valid" : "null") << '\n';
    // std::mutex mtx;
    std::cout << "header        : " << header << '\n';
    std::cout << "min_level     : " << static_cast<int>(min_level) << '\n';
    std::cout << "LLEWNL        : " << (last_log_ended_with_newline ? "true" : "false") << '\n';
    std::cout << "enable_color  : " << (enable_color ? "true" : "false") << '\n';
    std::cout << "config        : { ";
    for (const auto &option : config) {
        std::cout << static_cast<int>(option) << " ";
    }
    std::cout << "}\n";
    std::cout << "separator     : " << separator << '\n';
    std::cout << "buffer        : " << (buffer ? "valid" : "null") << '\n';
    std::cout << "buffer_length : " << buffer_length << '\n';
    std::cout << "fg_colors     : { ";
    for (std::size_t i = 0; i < 5; ++i) {
        std::cout << (fg_colors[i] ? "valid" : "null") << " ";
    }
    std::cout << "}\n";
    std::cout << "bg_colors     : { ";
    for (std::size_t i = 0; i < 5; ++i) {
        std::cout << (bg_colors[i] ? "valid" : "null") << " ";
    }
    std::cout << "}\n";
}

logger_t::~logger_t()
{
    std::free(buffer);
}

std::string logger_t::get_header() const
{
    return header;
}

log_level logger_t::get_log_level() const
{
    return min_level;
}

logger_t &logger_t::reset_colors()
{
    // Default foreground colors.
    fg_colors[debug]    = ansi::fg::cyan;
    fg_colors[info]     = ansi::fg::bright_white;
    fg_colors[warning]  = ansi::fg::bright_yellow;
    fg_colors[error]    = ansi::fg::red;
    fg_colors[critical] = ansi::fg::bright_red;
    // By default we do not have background colors.
    bg_colors[debug]    = quire::ansi::util::reset;
    bg_colors[info]     = quire::ansi::util::reset;
    bg_colors[warning]  = quire::ansi::util::reset;
    bg_colors[error]    = quire::ansi::util::reset;
    bg_colors[critical] = quire::ansi::util::reset;

    return *this;
}

logger_t &logger_t::set_file_handler(std::ostream *_fstream)
{
    fstream = _fstream;
    return *this;
}

logger_t &logger_t::set_output_stream(std::ostream *_ostream)
{
    ostream = _ostream;
    return *this;
}

logger_t &logger_t::set_header(std::string _header)
{
    header = _header;
    return *this;
}

logger_t &logger_t::set_log_level(log_level _level)
{
    min_level = _level;
    return *this;
}

logger_t &logger_t::set_separator(char _separator)
{
    separator = _separator;
    return *this;
}

logger_t &logger_t::set_color(log_level level, const char *fg, const char *bg)
{
    if ((level >= debug) && (level <= critical)) {
        fg_colors[level] = fg;
        bg_colors[level] = bg;
    }
    return *this;
}

void logger_t::format_message(char const *format, va_list args)
{
    if ((format == nullptr) || (format[0] == '\0')) {
        // Clean the buffer by setting it to an empty string.
        if (buffer != nullptr && buffer_length > 0) {
            buffer[0] = '\0';
        }
    } else {
        // Initialize variable argument lists to format the message.
        va_list length_args;
        va_copy(length_args, args);

        // Calculate the length of the formatted string.
        const int length = std::vsnprintf(nullptr, 0U, format, length_args);
        va_end(length_args);

        if (length > 0) {
            // Check if the buffer needs to be resized.
            if (buffer_length < static_cast<std::size_t>(length) + 1) {
                // Double the buffer length until it can hold the formatted string.
                while (buffer_length < static_cast<std::size_t>(length) + 1) {
                    buffer_length = buffer_length == 0 ? 128 : buffer_length * 2;
                }

                char *new_buffer = reinterpret_cast<char *>(std::realloc(buffer, buffer_length));
                if (new_buffer == nullptr) {
                    // Handle memory allocation failure.
                    perror("Failed to allocate memory for buffer resizing.");
                    exit(EXIT_FAILURE);
                }
                buffer = new_buffer;
            }

            // Format the message into the buffer.
            std::vsnprintf(buffer, buffer_length, format, args);
        }
    }
}

void logger_t::log(log_level level, char const *format, ...)
{
    // Ensure thread safety by locking the mutex.
    std::lock_guard<std::mutex> lock(mtx);

    if (level >= min_level) {
        // Format the message.
        va_list args;
        va_start(args, format);
        this->format_message(format, args);
        va_end(args);

        // Pass the level, location, and buffer to do_log.
        this->write_log(level, std::string(), buffer);
    }
}

void logger_t::log(log_level level, char const *file, int line, char const *format, ...)
{
    // Ensure thread safety by locking the mutex.
    std::lock_guard<std::mutex> lock(mtx);

    if (level >= min_level) {
        // Format the message.
        va_list args;
        va_start(args, format);
        this->format_message(format, args);
        va_end(args);

        // Pass the level, location, and buffer to do_log.
        this->write_log(level, __assemble_location(file, line), buffer);
    }
}

void logger_t::write_log(log_level level, const std::string &location, const char *content) const
{
    const char *start   = content;
    const char *newline = nullptr;

    // Split the content by lines and log each line individually.
    while ((newline = std::strchr(start, '\n')) != nullptr) {
        // Calculate the length of the line.
        std::size_t line_length = static_cast<std::size_t>(newline - start + 1);

        // Log the line with the current location.
        this->write_log_line(level, location, start, line_length);

        // Move to the next line.
        start = newline + 1;
    }

    // Log any remaining content after the last newline.
    if (*start != '\0') {
        this->write_log_line(level, location, start, std::strlen(start));
    }
}

void logger_t::write_log_line(log_level level, const std::string &location, const char *line, std::size_t length) const
{
    std::stringstream ss;

    // == LOG INFORMATION =====================================================
    // Add the header only if the previous log ended with a newline
    if (last_log_ended_with_newline) {
        if (!header.empty()) {
            ss << header << " " << separator << " ";
        }
        for (std::size_t i = 0; i < config.size(); ++i) {
            if (config[i] == option_t::level) {
                ss << __log_level_to_string(level) << " " << separator << " ";
            } else if (config[i] == option_t::date) {
                ss << __get_date() << " " << separator << " ";
            } else if (config[i] == option_t::time) {
                ss << __get_time() << " " << separator << " ";
            } else if ((config[i] == option_t::location) && !location.empty()) {
                ss << std::left << std::setw(16) << location << " " << separator << " ";
            }
        }
    }

    // Check that the line is not empty.
    if ((line != NULL) && (line[0] != '\0')) {
        // Write the actual log message.
        ss.write(line, static_cast<std::streamsize>(length));

        // Update the newline flag based on the current message's last character.
        last_log_ended_with_newline = (length > 0 && line[length - 1] == '\n');
    }

    // == WRITE TO FILE STREAM ================================================
    if (fstream) {
        (*fstream) << ss.str();
    }

    if (ostream) {
        // == COLOR (ON) ======================================================
        if (enable_color && (level >= debug) && (level <= critical)) {
            (*ostream) << bg_colors[level] << fg_colors[level];
        }

        // == WRITE STREAM ====================================================
        (*ostream) << ss.str();

        // == COLOR (OFF) =====================================================
        if (enable_color) {
            (*ostream) << ansi::util::reset;
        }
    }
}

} // namespace quire