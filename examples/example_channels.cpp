/// @file simple.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Simple example.

#include "quire/quire.hpp"
#include <quire/registry.hpp>

#include <memory>
#include <iostream>

void function_1()
{
    quire::registry_t::value_t pve = quire::registry.get("pve");
    qdebug(pve, "Hello %d", 10);
    qinfo(pve, "Hello %d", 10);
    qwarning(pve, "Hello %d", 10);
    qerror(pve, "Hello %d", 10);
    qcritical(pve, "Hello %d", 10);
}

void function_2()
{
    quire::registry_t::value_t pve = quire::registry.get("pve");
    qdebug(pve, "there %d", 10);
    qinfo(pve, "there %d", 10);
    qwarning(pve, "there %d", 10);
    qerror(pve, "there %d", 10);
    qcritical(pve, "there %d", 10);
}

void function_3()
{
    quire::registry_t::value_t pvp;
    try {
        pvp = quire::registry.get("pvp");
    } catch (quire::registry_exception_t const &re_get) {
        try {
            pvp = quire::registry.create("pvp", "pvp", quire::log_level::debug, '|');
        } catch (quire::registry_exception_t const &re_create) {
            std::cout << "Failed to create logger.\n";
            std::exit(1);
        }
    }
    qdebug(pvp, "there %d", 10);
    qinfo(pvp, "there %d", 10);
    qwarning(pvp, "there %d", 10);
    qerror(pvp, "there %d", 10);
    qcritical(pvp, "there %d", 10);
}

int main(int, char *[])
{
    quire::registry_t::value_t pve = quire::registry.create("pve", "pve", quire::log_level::debug, '|');

    pve->configure(quire::show_all);

    function_1();

    pve->configure(quire::show_nothing);

    function_2();

    function_3();

    quire::registry_t::value_t pvp = quire::registry.get("pvp");

    pvp->configure(quire::show_all);

    qcritical(pvp, "This is a critical message!");

    pvp->log(quire::critical, "This is a critical message!");

    return 0;
}