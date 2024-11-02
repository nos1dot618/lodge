#ifndef LODGE_H_
#define LODGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

static const LogLevel default_log_level = LOG_INFO;
static LogLevel current_log_level = default_log_level;
static FILE *log_filepath = NULL;
static pthread_mutex_t mutex;

#define lodge_assert(condition, fmt, ...)                                  \
    do {                                                                   \
        if (!(condition)) {                                                \
            fprintf(stderr, "Assertion Failed: "fmt"\n", ##__VA_ARGS__);   \
            exit(EXIT_FAILURE);                                            \
        }                                                                  \
    } while (0)

static const char *get_log_level_str(LogLevel level);
void lodge_constructor(const char *filepath);
void lodge_set_log_level(LogLevel level);
const char *lodge_get_current_timestamp();
void lodge_log_message(LogLevel level, const char *fmt, ...);
void lodge_destructor();

static const char *get_log_level_str(LogLevel level) {
    switch (level) {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_INFO:
        return "INFO";
    case LOG_WARNING:
        return "WARNING";
    case LOG_ERROR:
        return "ERROR";
    case LOG_FATAL:
        return "INFO";
    default:
        lodge_assert(0, "unexpected log level '%d'", level);
    }
}

void lodge_constructor(const char *filepath) {
    if (filepath) {
        log_filepath = fopen(filepath, "a");
        lodge_assert(log_filepath, "failed to open log file");
    }
    pthread_mutex_init(&mutex, NULL);
}

void lodge_set_log_level(LogLevel level) {
    current_log_level = level;
}

const char *lodge_get_current_timestamp() {
    static char buffer[20];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void lodge_log_message(LogLevel level, const char *fmt, ...) {
    // Skip logs below current log level
    if (level < current_log_level) {
        return;
    }

    // Lock mutex for thread safety
    pthread_mutex_lock(&mutex);

    const char *timestamp = lodge_get_current_timestamp();
    const char *log_level_str = get_log_level_str(level);
    if (log_filepath) {
        fprintf(log_filepath, "%s [%s]: ", timestamp, log_level_str);
        va_list args;
        va_start(args, fmt);
        vfprintf(log_filepath, fmt, args);
        va_end(args);
        fprintf(log_filepath, "\n");
        fflush(log_filepath);
    } else {
        fprintf(stdout, "%s [%s]: ", timestamp, log_level_str);
        va_list args;
        va_start(args, fmt);
        vfprintf(stdout, fmt, args);
        va_end(args);
        fprintf(stdout, "\n");
    }

    // Unlock mutex
    pthread_mutex_unlock(&mutex);
}

#define lodge_debug(fmt, ...) lodge_log_message(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define lodge_info(fmt, ...) lodge_log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define lodge_warning(fmt, ...) lodge_log_message(LOG_WARNING, fmt, ##__VA_ARGS__)
#define lodge_error(fmt, ...) lodge_log_message(LOG_ERROR, fmt, ##__VA_ARGS__)
// NOTE: calling lodge_destructor() in log_fatal may not be correct in thread context.
// As this will close the file and destroy the mutex which is common between threads,
// but calling fatal will only stop a single thread not the entire program.
#define lodge_fatal(fmt, ...)                                  \
    do {                                                       \
        lodge_log_message(LOG_FATAL, fmt, ##__VA_ARGS__);      \
        lodge_destructor();                                    \
        exit(EXIT_FAILURE);                                    \
    } while (0)

// Clean up logging resources
void lodge_destructor() {
    if (log_filepath) {
        fclose(log_filepath);
    }
    pthread_mutex_destroy(&mutex);
}

#endif // LODGE_H_
