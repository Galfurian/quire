/// @file example_quire.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/quire.hpp>

int main(int, char *[])
{
    quire::logger_t l0("l0", quire::log_level::debug, '|');
    l0.toggle_color(true);
    l0.configure(quire::logger_t::get_show_all_configuation());

    qdebug(l0, "Hello there!\n");
    qdebug(l0, "\n");
    qdebug(l0, "Empty\n");
    qdebug(l0, "\n");
    qdebug(
        l0, "The sun dipped below the horizon, casting\n"
            "a warm orange glow over the quiet village.\n"
            "Children laughed, chasing fireflies in the\n"
            "twilight as the stars began to appear.\n");
    qdebug(l0, "\n");
    qdebug(l0, "The sun dipped below the horizon, casting ");
    qdebug(l0, "a warm orange glow over the quiet village.\n");
    qdebug(l0, "Children laughed, chasing fireflies in the ");
    qdebug(l0, "twilight as the stars began to appear.\n");
    qdebug(l0, "\n");

    l0.configure({quire::option_t::level, quire::option_t::date, quire::option_t::location});
    qinfo(l0, "%2d\n", 10);

    l0.configure({quire::option_t::level, quire::option_t::time, quire::option_t::location});
    qwarning(l0, "%2d\n", 10);

    l0.configure({quire::option_t::level, quire::option_t::location});
    qerror(l0, "%2d\n", 10);

    l0.configure({quire::option_t::location});
    qcritical(l0, "%2d\n", 10);

    l0.configure({});
    qdebug(l0, "%.2f\n", 3.14);

    l0.set_log_level(quire::log_level::info);

    qdebug(l0, "%.2f\n", 3.14);

    l0.configure({quire::option_t::time});
    qinfo(l0, "%2d\n", 10);

    l0.set_header("");
    qinfo(l0, "%2d\n", 10);

    l0.set_color(quire::info, quire::ansi::fg::bright_magenta, quire::ansi::bg::white);

    l0.set_header("L0");
    l0.log(quire::info, "%2d\n", 10);

    return 0;
}