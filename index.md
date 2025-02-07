---
layout: default
---

# Quire

## Overview

[![Ubuntu](https://github.com/Galfurian/quire/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Galfurian/quire/actions/workflows/ubuntu.yml)
[![Windows](https://github.com/Galfurian/quire/actions/workflows/windows.yml/badge.svg)](https://github.com/Galfurian/quire/actions/workflows/windows.yml)
[![MacOS](https://github.com/Galfurian/quire/actions/workflows/macos.yml/badge.svg)](https://github.com/Galfurian/quire/actions/workflows/macos.yml)
[![Documentation](https://github.com/Galfurian/quire/actions/workflows/documentation.yml/badge.svg)](https://github.com/Galfurian/quire/actions/workflows/documentation.yml)

Quire is a flexible and efficient C++ logging library designed to make
application logging easy, configurable, and visually informative. With features
like ANSI color support, dynamic configurations, and customizable log levels,
Quire is suitable for projects of all scales.

---

## **Key Features**

- **Configurable Logging Options**: Include headers, timestamps, log levels, file locations, and more in log messages.
- **Log Levels**: Support for `debug`, `info`, `warning`, `error`, and `critical` levels, with customizable levels.
- **ANSI Color Support**: Add foreground and background colors to log levels for better visual distinction.
- **Thread-Safe Logging**: Ensure thread safety using internal mutex locks.
- **Logger Registry**: Optional system-wide logger management through a registry.
- **Format String Support**: Integrate formatted messages with `std::vsnprintf`.

---

## **Installation**

1. Clone the repository:

   ```bash
   git clone https://github.com/enrico-fraccaroli/quire.git
   ```

2. Include the `quire` directory in your project:

   ```cpp
   #include <quire/quire.hpp>
   ```

---

## **Quick Start**

### **Basic Example**

Here’s how to get started with a simple logger instance:

```cpp
#include <quire/quire.hpp>

int main() {
    quire::logger_t logger("ExampleLogger", quire::log_level::info, '|');
    logger.toggle_color(true); // Enable color output.
    logger.configure(quire::logger_t::get_show_all_configuation()); // Show all log details.

    qinfo(logger, "Welcome to Quire logging!\n");
    qdebug(logger, "Debugging information: x = %d\n", 42);
    qwarning(logger, "Warning: Low memory.\n");
    qerror(logger, "Error: Unable to open file.\n");
    qcritical(logger, "Critical: System shutting down!\n");

    return 0;
}
```

---

### **Advanced Features**

#### **Dynamic Configuration**

Customize the information displayed in log entries (e.g., headers, timestamps, locations):

```cpp
logger.configure({ quire::option_t::level, quire::option_t::time });
qinfo(logger, "Log entry with level and time.\n");

logger.configure({ quire::option_t::location });
qwarning(logger, "Log entry with file and line info.\n");
```

#### **Color Customization**

Set colors for specific log levels:

```cpp
logger.set_color(quire::info, quire::ansi::fg::bright_green, quire::ansi::bg::black);
qinfo(logger, "Custom colored log message.\n");
```

#### **Log Level Filtering**

Set the minimum log level to control verbosity:

```cpp
logger.set_log_level(quire::log_level::warning); // Only logs warning and above.
qdebug(logger, "This will not be logged.\n");
qwarning(logger, "This will be logged.\n");
```

---

## **Full Example**

```cpp
#include <quire/quire.hpp>

int main() {
    quire::logger_t logger("Example", quire::log_level::debug, '|');
    logger.toggle_color(true);
    logger.configure(quire::logger_t::get_show_all_configuation());

    qdebug(logger, "Hello there!\n");
    qinfo(logger, "Formatted value: %.2f\n", 3.14);

    logger.configure({ quire::option_t::level, quire::option_t::time });
    qwarning(logger, "Warning with time.\n");

    logger.set_log_level(quire::log_level::info);
    qdebug(logger, "This debug message will not appear.\n");
    qinfo(logger, "But this info message will.\n");

    logger.set_color(quire::info, quire::ansi::fg::bright_magenta, quire::ansi::bg::white);
    qinfo(logger, "Custom color info message.\n");

    return 0;
}
```

---

## **Using the Registry**

The registry allows system-wide logger management, including creation, retrieval, and removal of loggers.

```cpp
#include <quire/registry.hpp>

int main() {
    auto &logger = quire::create_logger(1, "RegistryLogger", quire::log_level::debug, '|');
    logger.toggle_color(true);
    qinfo(logger, "This logger is managed via the registry.\n");

    auto &retrieved_logger = quire::get_logger(1);
    qwarning(retrieved_logger, "Retrieved logger working fine.\n");

    quire::remove_logger(1); // Remove the logger.
    return 0;
}
```

---

## **API Reference**

### **Logger**

The `quire::logger_t` class provides core logging functionality:

- **Constructor**: `logger_t(std::string header, unsigned min_level, char separator)`
- **Configuration**:
  - `toggle_color(bool enable)`
  - `configure(const std::vector<option_t> &config)`
  - `set_log_level(unsigned level)`
  - `set_color(unsigned level, const char *fg, const char *bg)`
- **Logging Macros**:
  - `qdebug(logger, ...)`
  - `qinfo(logger, ...)`
  - `qwarning(logger, ...)`
  - `qerror(logger, ...)`
  - `qcritical(logger, ...)`

### **Registry**

The `quire::registry_t` class provides system-wide management of loggers:

- **Functions**:
  - `create_logger(key_t key, std::string header, unsigned min_level, char separator)`
  - `get_logger(key_t key)`
  - `remove_logger(key_t key)`
  - `loggers()`

---

## **License**

This library is licensed under the MIT License (see `LICENSE.md`).

---

## **Acknowledgments**

Created by Enrico Fraccaroli (<enry.frak@gmail.com>).