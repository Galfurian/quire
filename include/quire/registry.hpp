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
template <typename K>
class registry_t {
public:
    /// @brief The key associated with each logger.
    using key_t = K;
    /// @brief The way we store the loggers inside the map.
    using value_t = std::shared_ptr<logger_t>;
    /// @brief The map that stores the loggers.
    using map_t = std::map<key_t, value_t>;
    /// @brief An iterator for the map.
    using iterator = typename map_t::iterator;
    /// @brief An iterator for the map.
    using const_iterator = typename map_t::const_iterator;

    /// @brief Construct a new registry object.
    explicit registry_t()
        : m_map(),
          mtx()
    {
        // Nothing to do.
    }

    /// @brief Returns a copy of the loggers map.
    inline const map_t &loggers() const
    {
        return m_map;
    }

    /// @brief Creates a new logger.
    /// @param key the key associated with the logger.
    /// @param _header the header shown every time the log is printed.
    /// @param _min_level the minimal level we will print for this logger.
    /// @param _separator the semparator used for dividing all the log information.
    /// @return value_t the newly created logger.
    inline value_t create(key_t key, std::string _header, log_level _min_level, char _separator)
    {
        std::lock_guard<std::mutex> lock(mtx);
        // Check if the logger already exists.
        iterator it = m_map.find(key);
        if (it != m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` already exists.";
            throw quire::registry_exception_t(ss.str());
        }
        // Create the logger.
        std::shared_ptr<logger_t> logger = std::make_shared<logger_t>(_header, _min_level, _separator);
        // Insert the logger.
        if (!m_map.insert(std::make_pair(key, logger)).second) {
            std::stringstream ss;
            ss << "Failed to create logger `" << key << "`.";
            throw quire::registry_exception_t(ss.str());
        }
        return logger;
    }

    /// @brief Removes the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a copy of the logger.
    inline value_t remove(key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        // Check if the logger exists.
        iterator it = m_map.find(key);
        if (it == m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` does not exists.";
            throw quire::registry_exception_t(ss.str());
        }
        // Erase the entry.
        m_map.erase(it);
        return it->second;
    }

    /// @brief Clears the map from all the loggers.
    inline void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_map.clear();
    }

    /// @brief Checks if the registry contains the given logger.
    /// @param key the key associated with the logger.
    /// @return true if the logger exists, false otherwise.
    inline bool contains(key_t key) const
    {
        const_iterator it = m_map.find(key);
        return it != m_map.end();
    }

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    inline const value_t get(key_t key) const
    {
        // Check if the logger exists.
        const_iterator it = m_map.find(key);
        if (it == m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` does not exists.";
            throw quire::registry_exception_t(ss.str());
        }
        return it->second;
    }

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    inline value_t get(key_t key)
    {
        // Check if the logger exists.
        iterator it = m_map.find(key);
        if (it == m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` does not exists.";
            throw quire::registry_exception_t(ss.str());
        }
        return it->second;
    }

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    inline value_t operator[](key_t key)
    {
        // Check if the logger exists.
        iterator it = m_map.find(key);
        if (it == m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` does not exists.";
            throw quire::registry_exception_t(ss.str());
        }
        return it->second;
    }

    /// @brief Returns the logger associated with the given key.
    /// @param key the key associated with the logger.
    /// @return a reference to the logger.
    inline const value_t operator[](key_t key) const
    {
        // Check if the logger exists.
        const_iterator it = m_map.find(key);
        if (it == m_map.end()) {
            std::stringstream ss;
            ss << "Logger `" << key << "` does not exists.";
            throw quire::registry_exception_t(ss.str());
        }
        return it->second;
    }

    inline iterator begin() noexcept
    {
        return m_map.begin();
    }

    inline const_iterator begin() const noexcept
    {
        return m_map.begin();
    }

    inline iterator end() noexcept
    {
        return m_map.end();
    }

    inline const_iterator end() const noexcept
    {
        return m_map.end();
    }

    inline void adjust_header_length()
    {
        const_iterator cit;
        std::string::size_type max_length = 0;
        // Remove padding, and get the maximum length.
        for (cit = m_map.begin(); cit != m_map.end(); ++cit) {
            cit->second->set_header(this->trim(cit->second->get_header()));
            max_length = std::max(max_length, cit->second->get_header().length());
        }
        // Add padding to the headers, to align them.
        for (cit = m_map.begin(); cit != m_map.end(); ++cit) {
            cit->second->set_header(this->lalign(cit->second->get_header(), max_length, ' '));
        }
    }

    /// @brief Gets a singleton reference to the registry.
    inline static registry_t &instance()
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
template <typename K>
inline typename registry_t<K>::value_t get_logger(typename registry_t<K>::key_t key)
{
    return registry_t<K>::instance().get(key);
}

/// @brief Creates a new logger.
/// @param key the key associated with the logger.
/// @param _header the header shown every time the log is printed.
/// @param _min_level the minimal level we will print for this logger.
/// @param _separator the semparator used for dividing all the log information.
/// @return value_t the newly created logger.
template <typename K>
inline typename registry_t<K>::value_t create_logger(
    typename registry_t<K>::key_t key,
    std::string _header,
    quire::log_level _min_level,
    char _separator)
{
    registry_t<K> &registry                = registry_t<K>::instance();
    typename registry_t<K>::value_t logger = registry.create(key, _header, _min_level, _separator);
    registry.adjust_header_length();
    return logger;
}

/// @brief Removes the logger associated with the given key.
/// @param key the key associated with the logger.
/// @return a copy of the logger.
template <typename K>
inline typename registry_t<K>::value_t remove_logger(typename registry_t<K>::key_t key)
{
    registry_t<K> &registry                = registry_t<K>::instance();
    typename registry_t<K>::value_t logger = registry.remove(key);
    registry.adjust_header_length();
    return logger;
}

/// @brief Returns a copy of the loggers map.
template <typename K>
inline const typename registry_t<K>::map_t &get_loggers()
{
    return registry_t<K>::instance().loggers();
}

} // namespace quire
