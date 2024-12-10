#include "logger.h"

// Helper function to get the current timestamp as a string
const char* get_current_time() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    static char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return time_buffer;
}

// Function to write the log message to a file
void log_message(const char *log_file, LogLevel level, const char *message) {
    FILE *log_fp = fopen(log_file, "a");  // Open log file in append mode 
    if (log_fp == NULL) {
        perror("Failed to open log file");
        return;
    }

    // Get the current time
    const char *timestamp = get_current_time();

    // Map LogLevel enum to string
    const char *level_str;
    switch (level) {
        case INFO: level_str = "INFO"; break;
        case WARNING: level_str = "WARNING"; break;
        case ERROR: level_str = "ERROR"; break;
        default: level_str = "UNKNOWN"; break;
    }

    // Format the log message
    fprintf(log_fp, "[%s] [%s] %s\n", timestamp, level_str, message);

    fclose(log_fp);  // Close the file
}
