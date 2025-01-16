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

const char *quire::ansi::bg::bright_black   = "\33[40;1m";
const char *quire::ansi::bg::bright_red     = "\33[41;1m";
const char *quire::ansi::bg::bright_green   = "\33[42;1m";
const char *quire::ansi::bg::bright_yellow  = "\33[43;1m";
const char *quire::ansi::bg::bright_blue    = "\33[44;1m";
const char *quire::ansi::bg::bright_magenta = "\33[45;1m";
const char *quire::ansi::bg::bright_cyan    = "\33[46;1m";
const char *quire::ansi::bg::bright_white   = "\33[47;1m";

const char *quire::ansi::util::reset     = "\33[0m\033[49m";
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

/// @brief Combines filename and line number.
static inline std::string __assemble_location(const std::string &file, int line)
{
    std::stringstream ss;
    ss << line;
    return file.substr(file.find_last_of("/\\") + 1) + ":" + ss.str();
}

logger_t::logger_t(std::string _header, unsigned _min_level, char _separator, const std::vector<option_t> &_configuration) noexcept
    : ostream(&std::cout),
      fstream(NULL),
      mtx(),
      header(_header),
      min_level(_min_level),
      last_log_ended_with_newline(true),
      enable_color(true),
      configuration(_configuration),
      separator(_separator),
      buffer(nullptr),
      buffer_length(0),
      log_levels(),
      log_levels_max_name_length()
{
    this->initialize_default_levels();
}

logger_t::logger_t(logger_t &&other) noexcept
    : ostream(other.ostream),
      fstream(other.fstream),
      header(std::move(other.header)),
      min_level(other.min_level),
      last_log_ended_with_newline(other.last_log_ended_with_newline),
      enable_color(other.enable_color),
      configuration(std::move(other.configuration)),
      separator(std::move(other.separator)),
      buffer(other.buffer),
      buffer_length(other.buffer_length),
      log_levels(std::move(other.log_levels)),
      log_levels_max_name_length(other.log_levels_max_name_length)
{
    // Nullify moved-from resources in `other`.
    other.ostream       = nullptr;
    other.fstream       = nullptr;
    other.buffer        = nullptr;
    other.buffer_length = 0;
}

void logger_t::initialize_default_levels()
{
    this->add_or_update_log_level(debug, "DEBUG", ansi::fg::cyan, quire::ansi::util::reset);
    this->add_or_update_log_level(info, "INFO", ansi::fg::bright_white, quire::ansi::util::reset);
    this->add_or_update_log_level(warning, "WARNING", ansi::fg::bright_yellow, quire::ansi::util::reset);
    this->add_or_update_log_level(error, "ERROR", ansi::fg::red, quire::ansi::util::reset);
    this->add_or_update_log_level(critical, "CRITICAL", ansi::fg::bright_red, quire::ansi::util::reset);
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
    std::cout << "configuration : { ";
    for (const auto &option : configuration) {
        std::cout << static_cast<int>(option) << " ";
    }
    std::cout << "}\n";
    std::cout << "separator     : " << separator << '\n';
    std::cout << "buffer        : " << (buffer ? "valid" : "null") << '\n';
    std::cout << "buffer_length : " << buffer_length << '\n';
    std::cout << "log_levels     : { ";
    for (const auto &[level, properties] : log_levels) {
        std::cout << "    Level " << level << " (" << properties.name << "): "
                  << "Foreground: " << (properties.fg ? properties.fg : "default") << ", "
                  << "Background: " << (properties.bg ? properties.bg : "default") << '\n';
    }
    std::cout << "}\n";
}

logger_t::~logger_t()
{
    std::free(buffer);
}

void logger_t::clear_log_levels()
{
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety.
    log_levels.clear();                    // Remove all log levels from the map.
    log_levels_max_name_length = 0;
}

logger_t &logger_t::add_or_update_log_level(
    unsigned level,
    const char *name,
    const char *fg,
    const char *bg)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Check if the log level already exists.
    auto it = log_levels.find(level);
    if (it != log_levels.end()) {
        // Update the existing log level.
        it->second.name = name;
        it->second.fg   = fg;
        it->second.bg   = bg;
    } else {
        // Add a new log level.
        log_levels[level] = log_level_config_t{ name, fg, bg };
    }

    // Update max_level_name_length
    log_levels_max_name_length = std::max(log_levels_max_name_length, static_cast<int>(std::strlen(name)));

    return *this;
}

logger_t &logger_t::set_color(unsigned level, const char *fg, const char *bg)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto it = log_levels.find(level);
    if (it != log_levels.end()) {
        it->second.fg = fg;
        it->second.bg = bg;
    }

    return *this;
}

std::string logger_t::get_header() const
{
    return header;
}

unsigned logger_t::get_log_level() const
{
    return min_level;
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

logger_t &logger_t::set_log_level(unsigned _level)
{
    min_level = _level;
    return *this;
}

logger_t &logger_t::set_separator(char _separator)
{
    separator = _separator;
    return *this;
}

logger_t &logger_t::toggle_color(bool enable)
{
    enable_color = enable;
    return *this;
}

logger_t &logger_t::configure(const std::vector<option_t> &_configuration)
{
    configuration = _configuration;
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

void logger_t::log(unsigned level, char const *format, ...)
{
    // Ensure thread safety by locking the mutex.
    std::lock_guard<std::mutex> lock(mtx);

    // Check if the log level exists in the map.
    auto it = log_levels.find(level);

    // Check if we should log.
    if ((it != log_levels.end()) && (level >= min_level)) {
        // Format the message.
        va_list args;
        va_start(args, format);
        this->format_message(format, args);
        va_end(args);

        // Pass the level, location, and buffer to do_log.
        this->write_log(it->second, std::string(), buffer);
    }
}

void logger_t::log(unsigned level, char const *file, int line, char const *format, ...)
{
    // Ensure thread safety by locking the mutex.
    std::lock_guard<std::mutex> lock(mtx);

    // Check if the log level exists in the map.
    auto it = log_levels.find(level);

    // Check if we should log.
    if ((it != log_levels.end()) && (level >= min_level)) {
        // Format the message.
        va_list args;
        va_start(args, format);
        this->format_message(format, args);
        va_end(args);

        // Pass the level, location, and buffer to do_log.
        this->write_log(it->second, __assemble_location(file, line), buffer);
    }
}

void logger_t::write_log(const log_level_config_t &level, const std::string &location, const char *content) const
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

void logger_t::write_log_line(const log_level_config_t &level, const std::string &location, const char *line, std::size_t length) const
{
    std::stringstream ss;

    // == LOG INFORMATION =====================================================
    // Add the header only if the previous log ended with a newline
    if (last_log_ended_with_newline) {
        for (std::size_t i = 0; i < configuration.size(); ++i) {
            if ((configuration[i] == option_t::header) && !header.empty()) {
                ss << header << " " << separator << " ";
            } else if (configuration[i] == option_t::level) {
                ss << std::left << std::setw(log_levels_max_name_length) << level.name << " " << separator << " ";
            } else if (configuration[i] == option_t::date) {
                ss << __get_date() << " " << separator << " ";
            } else if (configuration[i] == option_t::time) {
                ss << __get_time() << " " << separator << " ";
            } else if ((configuration[i] == option_t::location) && !location.empty()) {
                ss << std::left << std::setw(16) << location << " " << separator << " ";
            }
        }
    }

    // Check that the line is not empty.
    if ((line != NULL) && (line[0] != '\0')) {
        // Write the actual log message.
        ss.write(line, static_cast<std::streamsize>(length));

        // Update the newline flag based on the current message's last character.
        last_log_ended_with_newline = (length > 0 && ((line[length - 1] == '\n') || (line[length - 1] == '\r')));
    }

    // == WRITE TO FILE STREAM ================================================
    if (fstream) {
        (*fstream) << ss.str();
    }

    if (ostream) {
        // == COLOR (ON) ======================================================
        if (enable_color) {
            (*ostream) << level.bg << level.fg;
        }

        // == WRITE STREAM ====================================================
        (*ostream) << ss.str();

        // == COLOR (OFF) =====================================================
        if (enable_color) {
            (*ostream) << ansi::util::reset << ansi::util::clearline;
        }
        (*ostream) << std::flush;
    }
}

} // namespace quire