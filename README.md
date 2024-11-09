> **Lodge** is a simple yet powerful C logging library that provides configurable log levels, file support, timestamping, and thread safety. It is designed to be easy to integrate into your projects and offers a variety of options for controlling log output and verbosity.
---

## Features
1. **Multiple Log Levels**: Supports `DEBUG`, `INFO`, `WARNING`, `ERROR`, and `FATAL` levels.
2. **Log to File**: Option to output logs to a file in addition to standard output.
3. **Timestamping**: Each log entry includes a timestamp to track when events occur.
4. **Thread Safety**: Uses a mutex to ensure logs are safe to use in multi-threaded applications.
5. **Dynamic Log Level Control**: Set the log level at runtime to control the verbosity.

## Getting Started
### Requirements
1. A C compiler (`GCC`, `Clang`, etc.)
2. POSIX-compliant environment (for `pthread` support)
### Installation
1. Include `lodge.h` into your project.
2. Compile the project with `libpthread`.
```bash
gcc your_program.c -o your_program -lpthread
```

## Usage
1. Include the library.
```c
#include "lodge.h"
```

2. Initialize the logging system with an optional file path. If a file path is provided, logs will be saved to the file.
```c
lodge_constructor(filepath); // Logs to a file
// or
lodge_constructor(NULL); // Logs to stdout
```

3. *(Optional)* Set desired log level to control verbosity.
```c
lodge_set_log_level(LOG_DEBUG); // Options: LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL
```

4. Use the provided macros to log messages at different levels:
```c
lodge_info("This is an informational message.");
lodge_debug("This is a debug message with value: %d", 42);
lodge_warning("This is a warning.");
lodge_error("An error occurred: %s", strerror(errno));
lodge_fatal("Fatal error, exiting...");
```

> [!IMPORTANT]
> `log_fatal` exits the program with `EXIT_FAILURE`, thus stopping the program execution.

5. *(Important)* Before exiting your application, call `lodge_destructor()` to close any open file handles and free resources.

## Example
```c
#include "lodge.h"

int main() {
    lodge_constructor(NULL);
    lodge_set_log_level(LOG_DEBUG);

    lodge_info("Application started");
    lodge_debug("Debugging value: %d", 100);
    lodge_warning("This is a warning message.");
    lodge_error("This is an error message.");
    lodge_fatal("This is a fatal error message.");

    lodge_destructor();
    return 0;
}
```
```console
2024-11-02 13:55:34 [INFO]: Application started
2024-11-02 13:55:34 [DEBUG]: Debugging value: 100
2024-11-02 13:55:34 [WARNING]: This is a warning message.
2024-11-02 13:55:34 [ERROR]: This is an error message.
2024-11-02 13:55:34 [FATAL]: This is a fatal error message.
```
