// MIT License
//
// Copyright (c) 2025 Lakshay Chauhan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Available at <https://raw.githubusercontent.com/nos1dot618/lodge/refs/heads/master/lodge.h>

/* Example Usage

#include "lodge.h"

int main() {
  lodge_constructor(NULL);
  // NOTE: This will log to stdout, log to a file by passing the filepath in
  //       the lodge_constructor.
  lodge_set_log_level(LOG_DEBUG);
  // NOTE: Disable logging of timestamp by calling lodge_toggle_timestamp API.
  // lodge_toggle_timestamp();
  lodge_info("Application started");
  lodge_debug("Debugging value: %d", 100);
  lodge_warning("This is a warning message.");
  lodge_error("This is an error message.");
  lodge_fatal("This is a fatal error message.");
  lodge_destructor();
  return 0;
}

*/

#ifndef LODGE_H_
#define LODGE_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t mutex;
#endif

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
static int log_timestamp = 1;

#define lodge_assert(condition, fmt, ...)                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, "Assertion Failed: " fmt "\n", ##__VA_ARGS__);           \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

static const char *get_log_level_str(LogLevel level);
void lodge_constructor(const char *filepath);
void lodge_set_log_level(LogLevel level);
const char *lodge_get_current_timestamp();
void lodge_log_message(LogLevel level, const char *fmt, ...);
void lodge_toggle_timestamp();
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
    return "FATAL";
  default:
    lodge_assert(0, "unexpected log level '%d'", level);
  }
}

void lodge_constructor(const char *filepath) {
  if (filepath) {
    log_filepath = fopen(filepath, "a");
    lodge_assert(log_filepath, "failed to open log file");
  }
#ifdef THREAD_SAFE
  pthread_mutex_init(&mutex, NULL);
#endif
}

void lodge_set_log_level(LogLevel level) { current_log_level = level; }

const char *lodge_get_current_timestamp() {
  static char buffer[20];
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  return buffer;
}

void lodge_log_message(LogLevel level, const char *fmt, ...) {
  if (level < current_log_level) {
    return;
  }
#ifdef THREAD_SAFE
  pthread_mutex_lock(&mutex);
#endif
  FILE *output = log_filepath ? log_filepath : stdout;
  if (log_timestamp) {
    fprintf(output, "%s ", lodge_get_current_timestamp());
  }
  fprintf(output, "[%s]: ", get_log_level_str(level));
  va_list args;
  va_start(args, fmt);
  vfprintf(output, fmt, args);
  va_end(args);
  fprintf(output, "\n");
  if (log_filepath) {
    fflush(log_filepath);
  }
#ifdef THREAD_SAFE
  pthread_mutex_unlock(&mutex);
#endif
}

#define lodge_debug(fmt, ...) lodge_log_message(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define lodge_info(fmt, ...) lodge_log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define lodge_warning(fmt, ...)                                                \
  lodge_log_message(LOG_WARNING, fmt, ##__VA_ARGS__)
#define lodge_error(fmt, ...) lodge_log_message(LOG_ERROR, fmt, ##__VA_ARGS__)
// NOTE: calling lodge_destructor() in log_fatal may not be correct in thread
//       context. As this will close the file and destroy the mutex which is
//       common between threads, but calling fatal will only stop a single
//       thread not the entire program.
#define lodge_fatal(fmt, ...)                                                  \
  do {                                                                         \
    lodge_log_message(LOG_FATAL, fmt, ##__VA_ARGS__);                          \
    lodge_destructor();                                                        \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

void lodge_toggle_timestamp() { log_timestamp = !log_timestamp; }

void lodge_destructor() {
  if (log_filepath) {
    fclose(log_filepath);
  }
#ifdef THREAD_SAFE
  pthread_mutex_destroy(&mutex);
#endif
}

#endif // LODGE_H_
