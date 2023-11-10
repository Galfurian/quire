/// @file simple.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Simple example.

#include <quire/quire.hpp>

int main(int, char *[])
{
    auto l0 = std::make_shared<quire::logger_t>("L0", quire::log_level::debug, '|');
    auto h0 = std::make_shared<quire::file_handler_t>("h0.log", "w");

    l0->set_file_handler(h0);

    l0->set_header("L0");
    l0->set_log_level(quire::log_level::debug);
    l0->toggle_color(true);
    l0->toggle_level(true);
    l0->toggle_date(true);
    l0->toggle_time(true);
    l0->toggle_location(true);

    qdebug(l0, "Hello there, I'm logging on file!");

    return 0;
}