/// @file example_quire.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/quire.hpp>

int main(int, char *[])
{
    quire::logger_t l0("l0", quire::log_level::debug, '|');

    l0.toggle_color(true);
    l0.toggle_level(true);
    l0.toggle_date(true);
    l0.toggle_time(true);
    l0.toggle_location(true);

    qdebug(l0, "Hello there!\n");
    qdebug(l0, "\n");
    qdebug(l0, "Empty\n");
    qdebug(l0, "\n");
    qdebug(l0, "The sun dipped below the horizon, casting\n"
               "a warm orange glow over the quiet village.\n"
               "Children laughed, chasing fireflies in the\n"
               "twilight as the stars began to appear.\n");
    qdebug(l0, "\n");
    qdebug(l0, "The sun dipped below the horizon, casting ");
    qdebug(l0, "a warm orange glow over the quiet village.\n");
    qdebug(l0, "Children laughed, chasing fireflies in the ");
    qdebug(l0, "twilight as the stars began to appear.\n");
    qdebug(l0, "\n");
    l0.toggle_date(true);
    l0.toggle_time(false);
    qinfo(l0, "%2d\n", 10);
    l0.toggle_date(false);
    l0.toggle_time(true);
    qwarning(l0, "%2d\n", 10);
    l0.toggle_date(false);
    l0.toggle_time(false);
    qerror(l0, "%2d\n", 10);
    l0.toggle_level(false);
    qcritical(l0, "%2d\n", 10);
    l0.toggle_location(false);
    qdebug(l0, "%.2f\n", 3.14);

    l0.set_log_level(quire::log_level::info);

    l0.toggle_location(false);
    qdebug(l0, "%.2f\n", 3.14);

    l0.toggle_time(true);
    qinfo(l0, "%2d\n", 10);

    l0.set_header("");
    qinfo(l0, "%2d\n", 10);

    l0.set_color(quire::info, quire::ansi::fg::bright_magenta, quire::ansi::bg::white);

    l0.set_header("L0");
    l0.log(quire::info, "%2d\n", 10);

    return 0;
}