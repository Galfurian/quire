/// @file registry.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/registry.hpp"

namespace quire
{

std::size_t registry_t::size() const
{
    return m_map.size();
}

bool registry_t::empty() const
{
    return m_map.empty();
}

registry_t::value_t registry_t::create(key_t key, std::string _header, log_level _min_level, char _separator)
{
    // Check if the logger already exists.
    iterator it = m_map.find(key);
    if (it != m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` already exists.");
    }
    // Create the logger.
    std::shared_ptr<logger_t> logger = std::make_shared<logger_t>(_header, _min_level, _separator);
    // Insert the logger.
    if (!m_map.insert(std::make_pair(key, logger)).second) {
        throw quire::registry_exception_t("Failed to create logger `" + key + "`.");
    }
    return logger;
}

registry_t::value_t registry_t::remove(key_t key)
{
    // Check if the logger exists.
    iterator it = m_map.find(key);
    if (it == m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` does not exists.");
    }
    // Erase the entry.
    m_map.erase(it);
    return it->second;
}

void registry_t::clear()
{
    m_map.clear();
}

bool registry_t::contains(key_t key) const
{
    const_iterator it = m_map.find(key);
    return it != m_map.end();
}

const registry_t::value_t registry_t::get(key_t key) const
{
    // Check if the logger exists.
    const_iterator it = m_map.find(key);
    if (it == m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` does not exists.");
    }
    return it->second;
}

registry_t::value_t registry_t::get(key_t key)
{
    // Check if the logger exists.
    iterator it = m_map.find(key);
    if (it == m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` does not exists.");
    }
    return it->second;
}

registry_t::value_t registry_t::operator[](key_t key)
{
    // Check if the logger exists.
    iterator it = m_map.find(key);
    if (it == m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` does not exists.");
    }
    return it->second;
}

const registry_t::value_t registry_t::operator[](key_t key) const
{
    // Check if the logger exists.
    const_iterator it = m_map.find(key);
    if (it == m_map.end()) {
        throw quire::registry_exception_t("Logger `" + key + "` does not exists.");
    }
    return it->second;
}

registry_t registry;

} // namespace quire