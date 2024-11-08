/// @file registry.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/registry.hpp"

namespace quire
{

registry_t::registry_t()
    : m_map(),
      mtx()
{
    // Nothing to do.
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

registry_t::value_t registry_t::insert(key_t key, value_t logger)
{
    std::lock_guard<std::mutex> lock(mtx);
    // Check if the logger already exists.
    iterator it = m_map.find(key);
    if (it != m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` already exists.";
        throw quire::registry_exception_t(ss.str());
    }
    // Insert the logger.
    if (!m_map.insert(std::make_pair(key, logger)).second) {
        std::stringstream ss;
        ss << "Failed to create logger `" << key << "`.";
        throw quire::registry_exception_t(ss.str());
    }
    return logger;
}

registry_t::value_t registry_t::remove(key_t key)
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
        std::stringstream ss;
        ss << "Logger `" << key << "` does not exists.";
        throw quire::registry_exception_t(ss.str());
    }
    return it->second;
}

registry_t::value_t registry_t::get(key_t key)
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

registry_t::value_t registry_t::operator[](key_t key)
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

const registry_t::value_t registry_t::operator[](key_t key) const
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

registry_t::iterator registry_t::begin() noexcept
{
    return m_map.begin();
}

registry_t::const_iterator registry_t::begin() const noexcept
{
    return m_map.begin();
}

registry_t::iterator registry_t::end() noexcept
{
    return m_map.end();
}

registry_t::const_iterator registry_t::end() const noexcept
{
    return m_map.end();
}

void registry_t::adjust_header_length()
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

} // namespace quire
