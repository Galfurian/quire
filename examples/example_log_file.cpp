/// @file example_log_file.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/quire.hpp>

#include <iostream>

int main(int, char *[])
{
    char log_filename[] = "h0.log";

    auto l0 = std::make_shared<quire::logger_t>("L0", quire::log_level::debug, '|');
    auto h0 = std::make_shared<quire::file_handler_t>(log_filename, "w");

    l0->set_file_handler(h0);
    l0->set_output_stream(nullptr);

    l0->set_header("L0");
    l0->set_log_level(quire::log_level::debug);
    l0->toggle_color(true);
    l0->toggle_level(true);
    l0->toggle_date(true);
    l0->toggle_time(true);
    l0->toggle_location(true);

    std::cout << "We are saving the log to `" << log_filename << "`.\n";

    qdebug(l0, "Hello there, I'm logging on file!");

    return 0;
}