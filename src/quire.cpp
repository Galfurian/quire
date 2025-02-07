/// @file quire.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/quire.hpp"

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace quire
{

namespace detail
{
/// @brief Get the current date in DD/MM/YY format.
/// @return The current date as a string.
static inline auto get_date() -> std::string
{
    try {
        // Get the current time
        auto now             = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        // Convert to local time
        std::tm timeinfo{};

#ifdef _WIN32
        // Windows: Use localtime_s (thread-safe)
        if (localtime_s(&timeinfo, &now_time) != 0) {
            std::cerr << "Error getting local time for date!\n";
            return "";
        }
#else
        // POSIX: Use localtime_r (thread-safe)
        if (localtime_r(&now_time, &timeinfo) == nullptr) {
            std::cerr << "Error getting local time for date!\n";
            return "";
        }
#endif

        // Format date
        std::ostringstream oss;
        oss << std::put_time(&timeinfo, "%d/%m/%y");
        return oss.str();

    } catch (const std::exception &e) {
        std::cerr << "Unexpected error getting date: " << e.what() << '\n';
        return "";
    }
}

/// @brief Get the current time in HH:MM format.
/// @return The current time as a string.
static inline auto get_time() -> std::string
{
    try {
        // Get the current time
        auto now             = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        // Convert to local time
        std::tm timeinfo{};

#ifdef _WIN32
        // Windows: Use localtime_s (thread-safe)
        if (localtime_s(&timeinfo, &now_time) != 0) {
            std::cerr << "Error getting local time for time!\n";
            return "";
        }
#else
        // POSIX: Use localtime_r (thread-safe)
        if (localtime_r(&now_time, &timeinfo) == nullptr) {
            std::cerr << "Error getting local time for time!\n";
            return "";
        }
#endif

        // Format time
        std::ostringstream oss;
        oss << std::put_time(&timeinfo, "%H:%M");
        return oss.str();

    } catch (const std::exception &e) {
        std::cerr << "Unexpected error getting time: " << e.what() << '\n';
        return "";
    }
}

} // namespace detail

logger_t::logger_t(
    std::string _header,
    unsigned _min_level,
    char _separator,
    const std::vector<option_t> &_configuration) noexcept
    : ostream(&std::cout)
    , fstream(nullptr)
    , header(std::move(std::move(_header)))
    , min_level(_min_level)
    , last_log_ended_with_newline(true)
    , enable_color(true)
    , configuration(_configuration)
    , separator(_separator)
    , buffer()
    , log_levels_max_name_length()
{
    this->initialize_default_levels();
}

logger_t::logger_t(logger_t &&other) noexcept
    : ostream(other.ostream)
    , fstream(other.fstream)
    , header(std::move(other.header))
    , min_level(other.min_level)
    , last_log_ended_with_newline(other.last_log_ended_with_newline)
    , enable_color(other.enable_color)
    , configuration(std::move(other.configuration))
    , separator(other.separator)
    , buffer(std::move(other.buffer))
    , log_levels(std::move(other.log_levels))
    , log_levels_max_name_length(other.log_levels_max_name_length)
{
    // Nullify moved-from resources in `other`.
    other.ostream = nullptr;
    other.fstream = nullptr;
}

auto logger_t::assemble_location(const std::string &file, int line) -> std::string
{
    std::stringstream ss;
    ss << line;
    return file.substr(file.find_last_of("/\\") + 1) + ":" + ss.str();
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
    std::cout << "ostream       : " << ((ostream != nullptr) ? "valid" : "null") << '\n';
    std::cout << "fstream       : " << ((fstream != nullptr) ? "valid" : "null") << '\n';
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
    std::cout << "buffer        : " << ((buffer.empty()) ? "valid" : "null") << '\n';
    std::cout << "log_levels     : { ";
    for (const auto &level : log_levels) {
        std::cout << "    Level " << level.first << " (" << level.second.name << "): "
                  << "Foreground: " << ((level.second.fg != nullptr) ? level.second.fg : "default") << ", "
                  << "Background: " << ((level.second.bg != nullptr) ? level.second.bg : "default") << '\n';
    }
    std::cout << "}\n";
}

void logger_t::clear_log_levels()
{
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety.
    log_levels.clear();                    // Remove all log levels from the map.
    log_levels_max_name_length = 0;
}

auto logger_t::add_or_update_log_level(unsigned level, const char *name, const char *fg, const char *bg) -> logger_t &
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
        log_levels[level] = log_level_config_t{name, fg, bg};
    }

    // Update max_level_name_length
    log_levels_max_name_length = std::max(log_levels_max_name_length, static_cast<int>(std::strlen(name)));

    return *this;
}

auto logger_t::set_color(unsigned level, const char *fg, const char *bg) -> logger_t &
{
    std::lock_guard<std::mutex> lock(mtx);

    auto it = log_levels.find(level);
    if (it != log_levels.end()) {
        it->second.fg = fg;
        it->second.bg = bg;
    }

    return *this;
}

auto logger_t::get_header() const -> std::string { return header; }

auto logger_t::get_log_level() const -> unsigned { return min_level; }

auto logger_t::set_file_handler(std::ostream *_fstream) -> logger_t &
{
    fstream = _fstream;
    return *this;
}

auto logger_t::set_output_stream(std::ostream *_ostream) -> logger_t &
{
    ostream = _ostream;
    return *this;
}

auto logger_t::set_header(std::string _header) -> logger_t &
{
    header = std::move(_header);
    return *this;
}

auto logger_t::set_log_level(unsigned _level) -> logger_t &
{
    min_level = _level;
    return *this;
}

auto logger_t::set_separator(char _separator) -> logger_t &
{
    separator = _separator;
    return *this;
}

auto logger_t::toggle_color(bool enable) -> logger_t &
{
    enable_color = enable;
    return *this;
}

auto logger_t::configure(const std::vector<option_t> &_configuration) -> logger_t &
{
    configuration = _configuration;
    return *this;
}

void logger_t::write_log(const log_level_config_t &level, const std::string &location) const
{
    std::size_t start       = 0;
    std::size_t newline_pos = 0;

    // Split the buffer by lines and log each line individually.
    while ((newline_pos = buffer.find('\n', start)) != std::string::npos) {
        // Extract the line including the newline character.
        std::string line = buffer.substr(start, newline_pos - start + 1);

        // Log the line with the current location.
        this->write_log_line(level, location, line, line.length());

        // Move to the next line.
        start = newline_pos + 1;
    }

    // Log any remaining content after the last newline.
    if (start < buffer.size()) {
        std::string remaining_line = buffer.substr(start);
        this->write_log_line(level, location, remaining_line, remaining_line.length());
    }
}

void logger_t::write_log_line(
    const log_level_config_t &level,
    const std::string &location,
    const std::string &line,
    std::size_t length) const
{
    std::stringstream ss;

    // == LOG INFORMATION =====================================================
    // Add the header only if the previous log ended with a newline
    if (last_log_ended_with_newline) {
        for (auto i : configuration) {
            if ((i == option_t::header) && !header.empty()) {
                ss << header << " " << separator << " ";
            } else if (i == option_t::level) {
                ss << std::left << std::setw(log_levels_max_name_length) << level.name << " " << separator << " ";
            } else if (i == option_t::date) {
                ss << detail::get_date() << " " << separator << " ";
            } else if (i == option_t::time) {
                ss << detail::get_time() << " " << separator << " ";
            } else if ((i == option_t::location) && !location.empty()) {
                ss << std::left << std::setw(16) << location << " " << separator << " ";
            }
        }
    }

    // Check that the line is not empty.
    if (!line.empty()) {
        // Write the actual log message.
        ss.write(line.c_str(), static_cast<std::streamsize>(length));

        // Update the newline flag based on the current message's last character.
        last_log_ended_with_newline = (line.back() == '\n') || (line.back() == '\r');
    }

    // == WRITE TO FILE STREAM ================================================
    if (fstream != nullptr) {
        (*fstream) << ss.str();
    }

    if (ostream != nullptr) {
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