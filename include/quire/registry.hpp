/// @file registry.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <exception>
#include <map>
#include <stdexcept>
#include <string>

#include "quire/quire.hpp"

namespace quire
{

/// @brief Represents a registry error.
class registry_exception_t : public std::runtime_error {
public:
    /// @brief Construct a new registry error.
    /// @param message the produced error message.
    explicit registry_exception_t(std::string message)
        : std::runtime_error(message)
    {
        // Nothing to do.
    }
};

/// @brief Registry for system wide loggers.
class registry_t {
public:
    /// @brief The key associated with each logger.
    using key_t          = std::string;
    /// @brief The way we store the loggers inside the map.
    using value_t        = std::shared_ptr<logger_t>;
    /// @brief The map that stores the loggers.
    using map_t          = std::map<key_t, value_t>;
    /// @brief An iterator for the map.
    using iterator       = typename map_t::iterator;
    /// @brief An iterator for the map.
    using const_iterator = typename map_t::const_iterator;

    /// @brief Construct a new registry object.
    explicit registry_t();

    /// @brief Returns a copy of the loggers map.
    const map_t &loggers() const;

    /// @brief Creates a new logger.
    /// @param key the key associated with the logger.
    /// @param _header the header shown every time the log is printed.
    /// @param _min_level the minimal level we will print for this logger.
    /// @param _separator the semparator used for dividing all the log information.
    /// @return value_t the newly created logger.
    value_t create(key_t key, std::string _header, log_level _min_level, char _separator);

    /// @brief Removes the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a copy of the logger.
    value_t remove(key_t key);

    /// @brief Clears the map from all the loggers.
    void clear();

    /// @brief Checks if the registry contains the given logger.
    /// @param key the key associated with the logger.
    /// @return true if the logger exists, false otherwise.
    bool contains(key_t key) const;

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    const value_t get(key_t key) const;

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    value_t get(key_t key);

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    value_t operator[](key_t key);

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    const value_t operator[](key_t key) const;

    /// @brief Gets a singleton reference to the registry.
    static registry_t &instance();

private:
    /// @brief Maps loggers name and their instance.
    map_t m_map;
    /// @brief A mutex to protect the use of the logger.
    std::mutex mtx;
};

/// @brief Returns the logger associated with the given key.
/// @param key the key associated with the logger.
/// @return a reference to the logger.
registry_t::value_t get_logger(registry_t::key_t key);

/// @brief Creates a new logger.
/// @param key the key associated with the logger.
/// @param _header the header shown every time the log is printed.
/// @param _min_level the minimal level we will print for this logger.
/// @param _separator the semparator used for dividing all the log information.
/// @return value_t the newly created logger.
registry_t::value_t create_logger(registry_t::key_t key, std::string _header, quire::log_level _min_level, char _separator);

/// @brief Removes the logger associated with the given key.
/// @param key the key associated with the logger.
/// @return a copy of the logger.
registry_t::value_t remove_logger(registry_t::key_t key);

/// @brief Returns a copy of the loggers map.
const registry_t::map_t &get_loggers();

} // namespace quire
