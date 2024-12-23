#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

/**
 * @brief Sets the minimum log level for filtering log messages.
 * @param level The minimum log level.
 */
void setLogLevel(LogLevel level);

/**
 * @brief Logs a simple message with priority, thread, and message content.
 * @param priority The priority level of the message (e.g., INFO, ERROR).
 * @param thread The thread or component generating the log.
 * @param message The log message content.
 */
void logMessage(LogLevel priority, const char* thread, const char* message);

/**
 * @brief Logs a formatted message with variable arguments.
 * @param level The priority level of the message (e.g., INFO, ERROR).
 * @param component The component generating the log.
 * @param format The format string for the log message.
 * @param ... Variable arguments for the format string.
 */
void logMessageFormatted(LogLevel level, const char *component, const char *format, ...);

/**
 * @brief Prints messages of a specific log level and higher.
 * @param level The log level to filter messages.
 */
void printLogMessages(LogLevel level);

#endif // LOGGER_H