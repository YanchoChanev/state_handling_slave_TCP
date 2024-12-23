#ifndef SLAVE_HANDLER_H
#define SLAVE_HANDLER_H

#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

/**
 * @brief Enum defining task IDs.
 */
typedef enum {
    COME_HANDLER_ID,                     ///< Communication handler ID.
    SLAVE_STATUS_OBSERVATION_HANDLER_ID, ///< Status observation handler ID.
    TASK_TEST_HANDLER_ID,                ///< Test task handler ID.
    TCP_ECHO_SERVER_TASK                 ///< Test task handler ID.
} TasksId;

/**
 * @brief Sets task handlers for the slave system.
 * @param taskHandlers Array of task handlers.
 */
void setTaskHandlers(TaskHandle_t *taskHandlers);

/**
 * @brief Handles communication tasks.
 * @param args Task arguments.
 */
// void vSlaveCommHandler(void *args);

/**
 * @brief Observes and retrieves the slave's status.
 * @param args Task arguments.
 */
// void vSlaveStatusObservationHandler(void *args);

/**
 * @brief Handles test tasks with user input.
 * @param args Task arguments.
 */
void vSlaveTaskTestHandling(void *args);

/**
 * @brief Handles restart signals.
 * @param args Task arguments.
 */
void vRestartHandler(void *args);

/**
 * @brief Handles restart signals.
 * @param args Task arguments.
 */
void vTCPEchoServerTask(void *args);

/**
 * @brief Handles master status check tasks.
 * @param args Task arguments.
 */
void vCreateTCPServer(void *args);
/**
 * @brief Handles master status check tasks.
 * @param args Task arguments.
 */
void vCreateTCPClient(void *args);

void vCreateTCPServerSimulation(void *args);

#endif // SLAVE_HANDLER_H
