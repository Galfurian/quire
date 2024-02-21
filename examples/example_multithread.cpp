/// @file example_multithread.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#include <quire/registry.hpp>

#include <condition_variable>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

enum channel_t {
    channel_local  = 10,
    channel_global = 20,
    channel_admin  = 30
};

struct product_t {
    int a;
    int b;
    bool ready;
};

std::condition_variable condition;
product_t product;
std::mutex mtx;

void producer_fun()
{
    auto local = quire::get_logger(channel_local);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    product.a     = 2;
    product.b     = 3;
    product.ready = true;
    qdebug(local, "a = %d, b = %d", product.a, product.b);
    condition.notify_one();
}

void consumer_fun()
{
    std::unique_lock<std::mutex> lock(mtx);
    auto global = quire::get_logger(channel_global);
    condition.wait(lock, [&]() { return product.ready; });
    qdebug(global, "%d * %d = %d", product.a, product.b, product.a * product.b);
}

int main(int, char *[])
{
    auto local  = quire::create_logger(channel_local, "local", quire::log_level::debug, '|');
    auto global = quire::create_logger(channel_global, "global", quire::log_level::debug, '|');
    auto admin  = quire::create_logger(channel_admin, "admin", quire::log_level::debug, '|');

    local->configure(quire::show_all);
    local->set_color(quire::debug, quire::ansi::fg::bright_cyan, quire::ansi::util::reset);

    global->configure(quire::show_all);
    global->set_color(quire::debug, quire::ansi::fg::bright_yellow, quire::ansi::util::reset);

    admin->configure(quire::show_all);
    admin->set_color(quire::debug, quire::ansi::fg::bright_red, quire::ansi::util::reset);

    std::thread producer(producer_fun);
    std::thread consumer(consumer_fun);

    producer.join();
    consumer.join();

    return 0;
}