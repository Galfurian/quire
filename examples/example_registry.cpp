/// @file simple.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Simple example.

#include "quire/quire.hpp"
#include <quire/registry.hpp>

#include <iostream>

enum channel_t {
    local,
    global,
    admin
};

using RegistryString = quire::registry_t<std::string>;
using RegistryInt    = quire::registry_t<int>;
using RegistryEnum   = quire::registry_t<channel_t>;

void registry_string()
{
    RegistryString::value_t logger = quire::get_logger<std::string>("rs");
    qdebug(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qinfo(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qwarning(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qerror(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qcritical(logger, "Hello %s, the temperature is %d.", "friend", 10);
}

void registry_int()
{
    RegistryInt::value_t logger = quire::get_logger<int>(0);
    qdebug(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qinfo(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qwarning(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qerror(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qcritical(logger, "Hello %s, the temperature is %d.", "friend", 10);
}

void registry_enum()
{
    RegistryEnum::value_t log_local = quire::get_logger<channel_t>(local);
    qdebug(log_local, "Hello %s, this is the local channel, the temperature is %d.", "friend", 10);
    RegistryEnum::value_t log_global = quire::get_logger<channel_t>(global);
    qdebug(log_global, "Hello %s, this is the global channel, the temperature is %d.", "friend", 10);
    RegistryEnum::value_t log_admin = quire::get_logger<channel_t>(admin);
    qdebug(log_admin, "Hello %s, this is the admin channel, the temperature is %d.", "friend", 10);
}

void registry_create()
{
    RegistryInt::value_t logger;
    try {
        logger = quire::get_logger<int>(1);
    } catch (quire::registry_exception_t const &) {
        try {
            logger = quire::create_logger<int>(1, "RegistryInt(1)", quire::log_level::debug, '|');
        } catch (quire::registry_exception_t const &) {
            std::cout << "Failed to create logger.\n";
            std::exit(1);
        }
    }
    logger->configure(quire::show_all);
    qdebug(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qinfo(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qwarning(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qerror(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qcritical(logger, "Hello %s, the temperature is %d.", "friend", 10);
}

int main(int, char *[])
{
    RegistryString::value_t rs = quire::create_logger<std::string>("rs", "RegistryString", quire::log_level::debug, '|');
    rs->configure(quire::show_all);
    registry_string();
    rs->configure(quire::show_nothing);
    registry_string();

    RegistryInt::value_t ri = quire::create_logger<int>(0, "RegistryInt(0)", quire::log_level::debug, '|');
    ri->configure(quire::show_all);
    registry_int();
    ri->configure(quire::show_nothing);
    registry_int();

    registry_create();

    quire::create_logger<channel_t>(local, "local", quire::log_level::debug, '|')->configure(quire::show_all);
    quire::create_logger<channel_t>(global, "global", quire::log_level::debug, '|')->configure(quire::show_all);
    quire::create_logger<channel_t>(admin, "admin", quire::log_level::debug, '|')->configure(quire::show_all);
    registry_enum();
    return 0;
}