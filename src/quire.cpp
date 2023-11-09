/// @file quire.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/quire.hpp"

#include <exception>
#include <stdexcept>

#include <cstdarg>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <exception>
#include <iomanip>
#include <iostream>
// #include <memory>
// #include <sstream>
// #include <string>

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
        return "debug";
    }
    if (level == info) {
        return "info";
    }
    if (level == warning) {
        return "warning";
    }
    if (level == error) {
        return "error";
    }
    return "critical";
}

/// @brief Combines filename and line number.
static inline std::string __assemble_location(const std::string &file, int line)
{
    return file.substr(file.find_last_of("/\\") + 1) + ":" + std::to_string(line);
}

file_handler_t::file_handler_t(const char *filename, const char *mode)
    : stream(fopen(filename, mode))
{
    // Nothing to do.
}

file_handler_t::~file_handler_t()
{
    fclose(stream);
}

std::size_t file_handler_t::write(const std::string &buffer)
{
    if (stream) {
        return fwrite(buffer.c_str(), buffer.size(), 1, stream);
    }
    return 0;
}

logger_t::logger_t(std::string _header, log_level _min_level, char _separator)
    : fhandler(),
      stream(&std::cout),
      mtx(),
      header(_header),
      min_level(_min_level),
      config(0),
      separator(_separator),
      buffer(nullptr),
      buffer_length(0)
{
    // Nothing to do.
}

logger_t::~logger_t()
{
    std::free(buffer);
}

void logger_t::set_file_handler(std::shared_ptr<file_handler_t> _fhandler)
{
    fhandler = _fhandler;
}

void logger_t::set_output_stream(std::ostream *_stream)
{
    stream = _stream;
}

void logger_t::set_header(std::string _header)
{
    header = _header;
}

void logger_t::set_log_level(log_level _level)
{
    min_level = _level;
}

void logger_t::set_separator(char _separator)
{
    separator = _separator;
}

void logger_t::configure(int _config)
{
    config = _config;
}

void logger_t::toggle_level(bool enable)
{
    (enable) ? (config |= show_level) : (config &= ~show_level);
}

void logger_t::toggle_color(bool enable)
{
    (enable) ? (config |= show_colored) : (config &= ~show_colored);
}

void logger_t::toggle_date(bool enable)
{
    (enable) ? (config |= show_date) : (config &= ~show_date);
}

void logger_t::toggle_time(bool enable)
{
    (enable) ? (config |= show_time) : (config &= ~show_time);
}

void logger_t::toggle_location(bool enable)
{
    (enable) ? (config |= show_location) : (config &= ~show_location);
}

void logger_t::log(log_level level, char const *format, ...)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (level >= min_level) {
        // == BUILD LOG =======================================================

        va_list length_args;
        va_start(length_args, format);
        va_list result_args;
        va_copy(result_args, length_args);
        const int length = std::vsnprintf(nullptr, 0U, format, length_args);
        if (length > 0) {
            if (buffer_length < static_cast<std::size_t>(length)) {
                buffer_length = static_cast<std::size_t>(length);
                buffer        = reinterpret_cast<char *>(std::realloc(buffer, buffer_length + 1));
            }
            std::vsnprintf(buffer, buffer_length, format, result_args);
        }
        va_end(result_args);
        va_end(length_args);

        // == LOGGING =========================================================

        this->do_log(level, std::string());
    }
}

void logger_t::log(log_level level, char const *file, int line, char const *format, ...)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (level >= min_level) {
        // == BUILD LOG =======================================================

        va_list length_args;
        va_start(length_args, format);
        va_list result_args;
        va_copy(result_args, length_args);
        const int length = std::vsnprintf(nullptr, 0U, format, length_args);
        if (length > 0) {
            if (buffer_length < static_cast<std::size_t>(length)) {
                buffer_length = static_cast<std::size_t>(length);
                buffer        = reinterpret_cast<char *>(std::realloc(buffer, buffer_length + 1));
            }
            std::vsnprintf(buffer, buffer_length, format, result_args);
        }
        va_end(result_args);
        va_end(length_args);

        // == LOGGING =========================================================

        this->do_log(level, __assemble_location(file, line));
    }
}

void logger_t::do_log(log_level level, const std::string &location) const
{
    // == FLAGS ==========================================================
    bool _show_level    = config & show_level;
    bool _show_date     = config & show_date;
    bool _show_time     = config & show_time;
    bool _show_colored  = config & show_colored;
    bool _show_location = config & show_location;

    // == STREAM ==========================================================
    std::stringstream ss;

    // == LOG =============================================================
    if (!header.empty()) {
        ss << header << " " << separator << " ";
    }
    if (_show_level) {
        ss << std::left << std::setw(9) << __log_level_to_string(level) << separator << " ";
    }
    if (_show_date) {
        ss << __get_date() << " ";
        if (!_show_time) {
            ss << separator << " ";
        }
    }
    if (_show_time) {
        ss << __get_time() << " " << separator << " ";
    }
    if (_show_location && !location.empty()) {
        ss << location << " " << separator << " ";
    }
    ss << buffer << "\n";

    // == COLOR (OFF) =====================================================

    // == WRITE STREAM ====================================================
    if (fhandler) {
        fhandler->write(ss.str());
    }

    // == COLOR (ON) ======================================================

    if (stream) {
        if (_show_colored) {
            if (level == critical) {
                (*stream) << ansi::fg::bright_red;
            } else if (level == error) {
                (*stream) << ansi::fg::red;
            } else if (level == warning) {
                (*stream) << ansi::fg::bright_yellow;
            } else if (level == info) {
                (*stream) << ansi::fg::bright_white;
            } else {
                (*stream) << ansi::fg::cyan;
            }
        }
        (*stream) << ss.str();
        if (_show_colored) {
            (*stream) << ansi::util::reset;
        }
    }
}

} // namespace quire