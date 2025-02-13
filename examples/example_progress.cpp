/// @file example_progress.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Demonstrates progress tracking using \r to overwrite the same line.

#include <chrono>
#include <thread>

#include <quire/quire.hpp>

int main()
{
    quire::logger_t logger("progress", quire::log_level::info, '|');
    // logger.toggle_color(false);

    const std::size_t total_steps = 100;

    for (std::size_t i = 0; i <= total_steps; ++i) {
        std::size_t bar_width = 50;
        std::size_t pos       = (i * bar_width) / total_steps;

        std::string progress_bar = "[" + std::string(pos, '=') + std::string(bar_width - pos, ' ') + "]";

        qinfo(logger, "%s %3d%%\r", progress_bar.c_str(), i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Print a newline after completion
    qinfo(logger, "\nDone!\n");

    return 0;
}
