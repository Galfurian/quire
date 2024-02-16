/// @file example_registry.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/registry.hpp>

#include <iostream>

enum channel_t {
    local  = 10,
    global = 20,
    admin  = 30
};

void registry_int()
{
    auto logger = quire::get_logger(0);
    qdebug(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qinfo(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qwarning(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qerror(logger, "Hello %s, the temperature is %d.", "friend", 10);
    qcritical(logger, "Hello %s, the temperature is %d.", "friend", 10);
}

void registry_enum()
{
    auto log_local = quire::get_logger(local);
    qdebug(log_local, "Hello %s, this is the local channel, the temperature is %d.", "friend", 10);
    auto log_global = quire::get_logger(global);
    qdebug(log_global, "Hello %s, this is the global channel, the temperature is %d.", "friend", 10);
    auto log_admin = quire::get_logger(admin);
    qdebug(log_admin, "Hello %s, this is the admin channel, the temperature is %d.", "friend", 10);
}

void registry_create()
{
    quire::registry_t::value_t logger;
    try {
        logger = quire::get_logger(1);
    } catch (quire::registry_exception_t const &) {
        try {
            logger = quire::create_logger(1, "RegistryInt(1)", quire::log_level::debug, '|');
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
    quire::registry_t::value_t ri = quire::create_logger(0, "RegistryInt(0)", quire::log_level::debug, '|');
    ri->configure(quire::show_all);
    registry_int();
    ri->configure(quire::show_nothing);
    registry_int();

    registry_create();

    quire::create_logger(local, "local", quire::log_level::debug, '|')->configure(quire::show_all);
    quire::create_logger(global, "global", quire::log_level::debug, '|')->configure(quire::show_all);
    quire::create_logger(admin, "admin", quire::log_level::debug, '|')->configure(quire::show_all);
    registry_enum();
    return 0;
}