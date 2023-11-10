/// @file registry.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/registry.hpp"
#include <mutex>

namespace quire
{

registry_t::registry_t()
    : m_map(),
      mtx()
{
    // Nothing to do.
}

registry_t &registry_t::instance()
{
    static registry_t registry;
    return registry;
}

std::size_t registry_t::size() const
{
    return m_map.size();
}

bool registry_t::empty() const
{
    return m_map.empty();
}

const registry_t::map_t &registry_t::loggers() const
{
    return m_map;
}

registry_t::value_t registry_t::create(key_t key, std::string _header, log_level _min_level, char _separator)
{
    std::lock_guard<std::mutex> lock(mtx);
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
    std::lock_guard<std::mutex> lock(mtx);
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
    std::lock_guard<std::mutex> lock(mtx);
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

registry_t::value_t get_logger(registry_t::key_t key)
{
    return registry_t::instance().get(key);
}

registry_t::value_t create_logger(registry_t::key_t key, std::string _header, quire::log_level _min_level, char _separator)
{
    return registry_t::instance().create(key, _header, _min_level, _separator);
}

registry_t::value_t remove_logger(registry_t::key_t key)
{
    return registry_t::instance().remove(key);
}

const registry_t::map_t &get_loggers()
{
    return registry_t::instance().loggers();
}

} // namespace quire