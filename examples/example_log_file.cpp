/// @file example_log_file.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/quire.hpp>

#include <iostream>

int main(int, char *[])
{
    char log_filename[] = "h0.log";

    quire::logger_t l0("L0", quire::log_level::debug, '|');

    std::ofstream file_stream(log_filename, std::ios::out | std::ios::app);
    if (!file_stream) {
        std::cerr << "Failed to open log file: " << log_filename << std::endl;
        return -1; // Handle the error appropriately
    }

    l0.set_output_stream(&file_stream);
    l0.set_output_stream(nullptr);

    l0.set_header("L0");
    l0.set_log_level(quire::log_level::debug);
    l0.configure(quire::configuration_show_all);

    std::cout << "We are saving the log to `" << log_filename << "`.\n";

    qdebug(l0, "Hello there, I'm logging on file!\n");

    return 0;
}