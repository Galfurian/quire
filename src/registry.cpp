/// @file registry.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include "quire/registry.hpp"

#include <string>
#include <utility>

namespace quire
{

namespace detail
{

/// @brief Trims specified characters from both the beginning and end of a string.
/// @param s The string to be trimmed.
/// @param padchar The characters to trim from the string.
/// @return std::string The trimmed string.
inline auto trim(const std::string &s, const std::string &padchar = " ") -> std::string
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
inline auto lalign(std::string s, std::string::size_type width, char fill = ' ') -> std::string
{
    std::string::size_type pad = (width > s.length()) ? (width - s.length()) : 0;
    return s.append(pad, fill);
}

} // namespace detail

auto registry_t::loggers() const -> const registry_t::map_t & { return m_map; }

auto registry_t::create(const registry_t::key_t key, std::string _header, unsigned _min_level, char _separator)
    -> registry_t::value_t &
{
    std::lock_guard<std::mutex> lock(mtx);

    // Check if the logger already exists.
    if (m_map.find(key) != m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` already exists.";
        throw quire::registry_exception_t(ss.str());
    }

    // Insert the logger directly into the map and retrieve a reference to it.
    auto insert_it = m_map.insert(std::make_pair(key, logger_t(std::move(_header), _min_level, _separator)));
    if (!insert_it.second) {
        std::stringstream ss;
        ss << "Failed to create logger `" << key << "`.";
        throw quire::registry_exception_t(ss.str());
    }

    // Adjust the header length.
    this->adjust_header_length();

    // Return a reference to the newly created logger.
    return insert_it.first->second;
}

void registry_t::remove(registry_t::key_t key)
{
    std::lock_guard<std::mutex> lock(mtx);
    // Check if the logger exists.
    auto it = m_map.find(key);
    if (it != m_map.end()) {
        m_map.erase(it);
        this->adjust_header_length();
    }
}

void registry_t::clear()
{
    std::lock_guard<std::mutex> lock(mtx);
    m_map.clear();
}

auto registry_t::contains(registry_t::key_t key) const -> bool
{
    auto it = m_map.find(key);
    return it != m_map.end();
}

auto registry_t::get(registry_t::key_t key) const -> const registry_t::value_t &
{
    // Check if the logger exists.
    auto it = m_map.find(key);
    if (it == m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` does not exists.";
        throw quire::registry_exception_t(ss.str());
    }
    return it->second;
}

auto registry_t::get(registry_t::key_t key) -> registry_t::value_t &
{
    // Check if the logger exists.
    auto it = m_map.find(key);
    if (it == m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` does not exists.";
        throw quire::registry_exception_t(ss.str());
    }
    return it->second;
}

auto registry_t::operator[](registry_t::key_t key) -> registry_t::value_t &
{
    // Check if the logger exists.
    auto it = m_map.find(key);
    if (it == m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` does not exists.";
        throw quire::registry_exception_t(ss.str());
    }
    return it->second;
}

auto registry_t::operator[](registry_t::key_t key) const -> const registry_t::value_t &
{
    // Check if the logger exists.
    auto it = m_map.find(key);
    if (it == m_map.end()) {
        std::stringstream ss;
        ss << "Logger `" << key << "` does not exists.";
        throw quire::registry_exception_t(ss.str());
    }
    return it->second;
}

auto registry_t::begin() noexcept -> registry_t::iterator { return m_map.begin(); }

auto registry_t::begin() const noexcept -> registry_t::const_iterator { return m_map.begin(); }

auto registry_t::end() noexcept -> registry_t::iterator { return m_map.end(); }

auto registry_t::end() const noexcept -> registry_t::const_iterator { return m_map.end(); }

void registry_t::adjust_header_length()
{
    iterator cit;
    std::string::size_type max_length = 0;

    // Remove padding and get the maximum length.
    for (cit = m_map.begin(); cit != m_map.end(); ++cit) {
        cit->second.set_header(detail::trim(cit->second.get_header()));
        max_length = std::max(max_length, cit->second.get_header().length());
    }

    // Add padding to the headers to align them.
    for (cit = m_map.begin(); cit != m_map.end(); ++cit) {
        cit->second.set_header(detail::lalign(cit->second.get_header(), max_length, ' '));
    }
}

} // namespace quire
