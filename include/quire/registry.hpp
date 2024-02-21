/// @file registry.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

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
    using key_t = std::uint64_t;
    /// @brief The way we store the loggers inside the map.
    using value_t = std::shared_ptr<logger_t>;
    /// @brief The map that stores the loggers.
    using map_t = std::map<key_t, value_t>;
    /// @brief An iterator for the map.
    using iterator = typename map_t::iterator;
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

    /// @brief Inserts an existing logger in the registry.
    /// @param key the key associated with the logger.
    /// @param logger the logger.
    /// @return value_t the inserted logger.
    value_t insert(key_t key, value_t logger);

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

    iterator begin() noexcept;

    const_iterator begin() const noexcept;

    iterator end() noexcept;

    const_iterator end() const noexcept;

    void adjust_header_length();

    /// @brief Gets a singleton reference to the registry.
    static inline registry_t &instance()
    {
        static registry_t registry;
        return registry;
    }

private:
    /// @brief Removes the specified characters from both the beginning and the end of the string.
    /// @param s the input string.
    /// @param padchar the char that should be removed.
    /// @return the trimmed string.
    inline std::string trim(const std::string &s, const std::string &padchar = " ")
    {
        std::string::size_type left  = s.find_first_not_of(padchar);
        std::string::size_type right = s.find_last_not_of(padchar);
        return (left != std::string::npos) ? s.substr(left, right - left + 1) : "";
    }

    /// @brief Aligns the string to the left.
    /// @param s the input string.
    /// @param width the full lenght of the final string.
    /// @param fill the char used to fill the tring.
    /// @return the aligned string.
    inline std::string lalign(std::string s, std::string::size_type width, char fill = ' ')
    {
        std::string::size_type pad = (width > s.length()) ? (width - s.length()) : 0;
        return s.append(pad, fill);
    }

    /// @brief Maps loggers name and their instance.
    map_t m_map;
    /// @brief A mutex to protect the use of the logger.
    std::mutex mtx;
};

/// @brief Returns the logger associated with the given key.
/// @param key the key associated with the logger.
/// @return a reference to the logger.
inline registry_t::value_t get_logger(registry_t::key_t key)
{
    return registry_t::instance().get(key);
}

/// @brief Creates a new logger.
/// @param key the key associated with the logger.
/// @param _header the header shown every time the log is printed.
/// @param _min_level the minimal level we will print for this logger.
/// @param _separator the semparator used for dividing all the log information.
/// @return value_t the newly created logger.
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

/// @brief Inserts an existing logger in the registry.
/// @param key the key associated with the logger.
/// @param logger the logger.
/// @return value_t the inserted logger.
inline registry_t::value_t insert_logger(registry_t::key_t key, registry_t::value_t logger)
{
    registry_t &registry = registry_t::instance();
    registry.insert(key, logger);
    registry.adjust_header_length();
    return logger;
}

/// @brief Removes the logger associated with the given key.
/// @param key the key associated with the logger.
/// @return a copy of the logger.
inline registry_t::value_t remove_logger(registry_t::key_t key)
{
    registry_t &registry       = registry_t::instance();
    registry_t::value_t logger = registry.remove(key);
    registry.adjust_header_length();
    return logger;
}

/// @brief Returns a copy of the loggers map.
inline const registry_t::map_t &loggers()
{
    return registry_t::instance().loggers();
}

} // namespace quire
