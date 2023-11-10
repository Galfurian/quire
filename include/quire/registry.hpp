/// @file registry.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

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
    /// @param key the key that produced the error.
    explicit registry_exception_t(std::string message)
        : std::runtime_error(message)
    {
        // Nothing to do.
    }
};

class registry_t {
public:
    using key_t          = std::string;
    using value_t        = std::shared_ptr<logger_t>;
    using map_t          = std::map<key_t, value_t>;
    using iterator       = typename map_t::iterator;
    using const_iterator = typename map_t::const_iterator;

    explicit registry_t();

    std::size_t size() const;

    bool empty() const;
    
    const map_t &loggers() const;

    value_t create(key_t key, std::string _header, log_level _min_level, char _separator);

    value_t remove(key_t key);

    void clear();

    bool contains(key_t key) const;

    const value_t get(key_t key) const;

    value_t get(key_t key);

    value_t operator[](key_t key);

    const value_t operator[](key_t key) const;

    static registry_t &instance();

private:
    /// @brief Maps loggers name and their instance.
    map_t m_map;
    /// @brief A mutex to protect the use of the logger.
    std::mutex mtx;
};

registry_t::value_t get_logger(registry_t::key_t key);

registry_t::value_t create_logger(registry_t::key_t key, std::string _header, quire::log_level _min_level, char _separator);

registry_t::value_t remove_logger(registry_t::key_t key);

const registry_t::map_t &get_loggers();

} // namespace quire
