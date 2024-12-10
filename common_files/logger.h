#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LOG_FILE_PATH "./Logs/logfile.log" // Default path to the log file (can be passed dynamically)

typedef enum {
    INFO,
    WARNING,
    ERROR
} LogLevel;

// Function prototypes
void log_message(const char *log_file, LogLevel level, const char *message);

#endif
