#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define LOG_FILE "slave_log.txt"
#define PRIORITY_LEVEL LOG_LEVEL_INFO

// Global variable to store the minimum log level
static LogLevel currentLogLevel = LOG_LEVEL_DEBUG;

void setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void logMessage(LogLevel priority, const char* thread, const char* message) {
    if (priority < currentLogLevel) {
        return; // Do not log messages below the current log level
    }

    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    char* timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline character

    const char* priorityStr;
    switch (priority) {
        case LOG_LEVEL_DEBUG: priorityStr = "DEBUG"; break;
        case LOG_LEVEL_INFO: priorityStr = "INFO"; break;
        case LOG_LEVEL_WARN: priorityStr = "WARN"; break;
        case LOG_LEVEL_ERROR: priorityStr = "ERROR"; break;
        default: priorityStr = "UNKNOWN"; break;
    }

    if(priority >= PRIORITY_LEVEL){ 
        fprintf(logFile, "[%s] [%s] [%s] %s\n", timestamp, priorityStr, thread, message);
    }
    fclose(logFile);
}

void logMessageFormatted(LogLevel level, const char *component, const char *format, ...) {
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    logMessage(level, component, message);
}

void printLogMessages(LogLevel level) {
    FILE* logFile = fopen(LOG_FILE, "r");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), logFile) != NULL) {
        LogLevel messageLevel;
        if (strstr(line, "[DEBUG]")) {
            messageLevel = LOG_LEVEL_DEBUG;
        } else if (strstr(line, "[INFO]")) {
            messageLevel = LOG_LEVEL_INFO;
        } else if (strstr(line, "[WARN]")) {
            messageLevel = LOG_LEVEL_WARN;
        } else if (strstr(line, "[ERROR]")) {
            messageLevel = LOG_LEVEL_ERROR;
        } else {
            messageLevel = LOG_LEVEL_DEBUG; // Default to DEBUG if level is unknown
        }

        if (messageLevel >= level) {
            printf("%s", line);
        }
    }

    fclose(logFile);
}
