/// @file simple.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Simple example.

#include <memory>
#include <quire/quire.hpp>

int main(int, char *[])
{
    auto l0 = std::make_shared<quire::logger_t>("L0", quire::log_level::debug, '|');
    auto l1 = std::make_shared<quire::logger_t>("L1", quire::log_level::debug, '|');
    auto h0 = std::make_shared<quire::file_handler_t>("h0.log", "w");
    auto h1 = std::make_shared<quire::file_handler_t>("h1.log", "w");

    l0->set_file_handler(h0);
    l1->set_file_handler(h1);

    l0->set_header("L0");
    l0->set_log_level(quire::log_level::debug);
    l0->toggle_color(true);
    l0->toggle_level(true);
    l0->toggle_date(true);
    l0->toggle_time(true);
    l0->toggle_location(true);

    qdebug(l0, "Hello there!");
    l0->toggle_date(true);
    l0->toggle_time(false);
    qinfo(l0, "%2d", 10);
    l0->toggle_date(false);
    l0->toggle_time(true);
    qwarning(l0, "%2d", 10);
    l0->toggle_date(false);
    l0->toggle_time(false);
    qerror(l0, "%2d", 10);
    l0->toggle_level(false);
    qcritical(l0, "%2d", 10);
    l0->toggle_location(false);
    qdebug(l0, "%.2f", 3.14);

    l0->set_log_level(quire::log_level::info);

    l0->toggle_location(false);
    qdebug(l0, "%.2f", 3.14);

    l0->toggle_time(true);
    qinfo(l0, "%2d", 10);

    l0->set_header("");
    qinfo(l0, "%2d", 10);

    l0->set_header("L0");
    l0->log(quire::info, "%2d", 10);

    return 0;
}