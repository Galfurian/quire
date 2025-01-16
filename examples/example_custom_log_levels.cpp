/// @file example_custom_log_levels.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Example demonstrating the quire logging system with custom log levels.

#include <quire/quire.hpp>

int main(int, char *[])
{
    // Create a logger with initial configuration.
    quire::logger_t l0("l0", quire::log_level::debug, '|');
    l0.toggle_color(true);
    l0.configure(quire::logger_t::get_show_all_configuation());

    // Clear the existing log levels.
    l0.clear_log_levels();

    // Add 8 custom log levels.
    l0.add_or_update_log_level(0, "DEBUG", quire::ansi::fg::green, quire::ansi::util::reset);
    l0.add_or_update_log_level(1, "INFO", quire::ansi::fg::cyan, quire::ansi::bg::white);
    l0.add_or_update_log_level(2, "NOTICE", quire::ansi::fg::blue, quire::ansi::util::reset);
    l0.add_or_update_log_level(3, "WARNING", quire::ansi::fg::yellow, quire::ansi::util::reset);
    l0.add_or_update_log_level(4, "ERROR", quire::ansi::fg::red, quire::ansi::util::reset);
    l0.add_or_update_log_level(5, "CRITICAL", quire::ansi::fg::bright_red, quire::ansi::util::reset);
    l0.add_or_update_log_level(6, "ALERT", quire::ansi::fg::bright_yellow, quire::ansi::util::reset);
    l0.add_or_update_log_level(7, "EMERGENCY", quire::ansi::fg::white, quire::ansi::util::reset);

    // Test each custom log level with a message.
    l0.log(0, "This is a debug message.\n");
    l0.log(1, "This is an info message.\n");
    l0.log(2, "This is a notice message.\n");
    l0.log(3, "This is a warning message.\n");
    l0.log(4, "This is an error message.\n");
    l0.log(5, "This is a critical message.\n");
    l0.log(6, "This is an alert message.\n");
    l0.log(7, "This is an emergency message.\n");

    // // Change configuration to include fewer options and test.
    // l0.configure({ quire::option_t::level, quire::option_t::time });
    // l0.log(1, "This is another info message with time.\n");

    // // Test setting a custom header and color updates.
    // l0.set_header("CustomLogger");
    // l0.set_color(quire::info, quire::ansi::fg::bright_magenta, quire::ansi::bg::white);
    // l0.log(1, "Info with updated header and colors.\n");

    // // Test disabling colors.
    // l0.toggle_color(false);
    // l0.log(6, "Alert without colors.\n");

    return 0;
}