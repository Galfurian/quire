/// @file registry.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the registry class and associated functions for managing
/// system-wide loggers.

#pragma once

#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>
#include <mutex>
#include <map>

#include "quire/quire.hpp"

namespace quire
{

/// @brief Represents an exception specific to registry operations.
class registry_exception_t : public std::runtime_error {
public:
    /// @brief Constructs a new registry exception with a specific error message.
    /// @param message The error message describing the exception.
    explicit registry_exception_t(std::string message)
        : std::runtime_error(message)
    {
        // Nothing to do.
    }
};

/// @brief Manages system-wide loggers and provides utilities for logger
/// registration, lookup, and removal.
class registry_t {
public:
    /// @brief The unique identifier associated with each logger.
    using key_t = std::uint64_t;
    /// @brief The type used to store logger instances in the registry.
    using value_t = std::shared_ptr<logger_t>;
    /// @brief The map structure that associates each key with a logger instance.
    using map_t = std::map<key_t, value_t>;
    /// @brief An iterator type for non-constant access to the logger map.
    using iterator = typename map_t::iterator;
    /// @brief An iterator type for constant access to the logger map.
    using const_iterator = typename map_t::const_iterator;

    /// @brief Construct a new registry object.
    explicit registry_t();

    /// @brief Returns a copy of the loggers map.
    const map_t &loggers() const;

    /// @brief Creates a new logger and adds it to the registry.
    /// @param key The key associated with the logger.
    /// @param _header The header displayed with each log entry.
    /// @param _min_level The minimum log level required for messages to be logged.
    /// @param _separator The separator character between log components.
    /// @return value_t A shared pointer to the newly created logger.
    value_t create(key_t key, std::string _header, log_level _min_level, char _separator);

    /// @brief Inserts an existing logger into the registry.
    /// @param key The key associated with the logger.
    /// @param logger The logger instance to insert.
    /// @return value_t A shared pointer to the inserted logger.
    value_t insert(key_t key, value_t logger);

    /// @brief Removes the logger associated with the given key.
    /// @param key The key associated with the logger.
    /// @return value_t A copy of the logger that was removed from the registry.
    value_t remove(key_t key);

    /// @brief Clears all loggers from the registry.
    void clear();

    /// @brief Checks if a logger with the specified key exists in the registry.
    /// @param key The key associated with the logger.
    /// @return true if the logger exists; false otherwise.
    bool contains(key_t key) const;

    /// @brief Retrieves the logger associated with the specified key.
    /// @param key The key associated with the logger.
    /// @return value_t A shared pointer to the logger, or throws if not found.
    const value_t get(key_t key) const;

    /// @brief Retrieves a non-const logger associated with the specified key.
    /// @param key The key associated with the logger.
    /// @return value_t A shared pointer to the logger, or throws if not found.
    value_t get(key_t key);

    /// @brief Accesses the logger associated with the given key using the subscript operator.
    /// @param key The key associated with the logger.
    /// @return value_t A shared pointer to the logger instance.
    value_t operator[](key_t key);

    /// @brief Accesses the logger associated with the given key using the subscript operator.
    /// @param key The key associated with the logger.
    /// @return const value_t A shared pointer to the logger instance.
    const value_t operator[](key_t key) const;

    /// @brief Returns an iterator to the beginning of the logger map.
    /// @return iterator An iterator to the beginning of the map.
    iterator begin() noexcept;

    /// @brief Returns a const iterator to the beginning of the logger map.
    /// @return const_iterator A const iterator to the beginning of the map.
    const_iterator begin() const noexcept;

    /// @brief Returns an iterator to the end of the logger map.
    /// @return iterator An iterator to the end of the map.
    iterator end() noexcept;

    /// @brief Returns a const iterator to the end of the logger map.
    /// @return const_iterator A const iterator to the end of the map.
    const_iterator end() const noexcept;

    /// @brief Adjusts the header length for consistent alignment in logs.
    void adjust_header_length();

    /// @brief Retrieves the singleton instance of the registry.
    /// @return registry_t& A reference to the singleton registry instance.
    static inline registry_t &instance()
    {
        static registry_t registry;
        return registry;
    }

private:
    /// @brief Trims specified characters from both the beginning and end of a string.
    /// @param s The string to be trimmed.
    /// @param padchar The characters to trim from the string.
    /// @return std::string The trimmed string.
    inline std::string trim(const std::string &s, const std::string &padchar = " ")
    {
        std::string::size_type left  = s.find_first_not_of(padchar);
        std::string::size_type right = s.find_last_not_of(padchar);
        return (left != std::string::npos) ? s.substr(left, right - left + 1) : "";
    }

    /// @brief Left-aligns a string within a specified width.
    /// @param s The string to align.
    /// @param width The total width for alignment.
    /// @param fill The character used for padding the string.
    /// @return std::string The aligned string.
    inline std::string lalign(std::string s, std::string::size_type width, char fill = ' ')
    {
        std::string::size_type pad = (width > s.length()) ? (width - s.length()) : 0;
        return s.append(pad, fill);
    }

    /// @brief Stores the mapping between logger keys and logger instances.
    map_t m_map;
    /// @brief A mutex ensuring thread-safe access to the logger registry.
    std::mutex mtx;
};

/// @brief Retrieves a logger by key from the registry.
/// @param key The key associated with the logger.
/// @return value_t A shared pointer to the requested logger.
inline registry_t::value_t get_logger(registry_t::key_t key)
{
    return registry_t::instance().get(key);
}

/// @brief Creates a new logger in the registry.
/// @param key The key associated with the logger.
/// @param _header The header for log messages.
/// @param _min_level The minimum logging level.
/// @param _separator The separator character for log entries.
/// @return value_t A shared pointer to the created logger.
inline registry_t::value_t create_logger(
    registry_t::key_t key,
    std::string _header,
    quire::log_level _min_level,
    char _separator)
{
    registry_t &registry       = registry_t::instance();
    registry_t::value_t logger = registry.create(key, _header, _min_level, _separator);
    registry.adjust_header_length();
    return logger;
}

/// @brief Inserts a logger into the registry.
/// @param key The key associated with the logger.
/// @param logger The logger instance to insert.
/// @return value_t A shared pointer to the inserted logger.
inline registry_t::value_t insert_logger(registry_t::key_t key, registry_t::value_t logger)
{
    registry_t &registry = registry_t::instance();
    registry.insert(key, logger);
    registry.adjust_header_length();
    return logger;
}

/// @brief Removes a logger from the registry by key.
/// @param key The key associated with the logger.
/// @return value_t A shared pointer to the removed logger.
inline registry_t::value_t remove_logger(registry_t::key_t key)
{
    registry_t &registry       = registry_t::instance();
    registry_t::value_t logger = registry.remove(key);
    registry.adjust_header_length();
    return logger;
}

/// @brief Returns a const reference to the map of loggers in the registry.
/// @return const map_t& The map of registered loggers.
inline const registry_t::map_t &loggers()
{
    return registry_t::instance().loggers();
}

} // namespace quire
