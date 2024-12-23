#ifndef SLAVE_RESTART_THREADS_H
#define SLAVE_RESTART_THREADS_H

#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

/**
 * @brief Structure defining a task handler.
 */
typedef struct {
    uint8_t id;                  ///< Task identifier.
    TaskFunction_t taskFunction; ///< Task function pointer.
    const char *taskName;        ///< Task name.
    uint8_t taskPrio;    ///< Task handler reference.
    TaskHandle_t taskHandler;    ///< Task handler reference.
} TaskHandler;

/**
 * @brief Number of task handlers.
 */
#define TAKS_HADLERS_SIZE 4

/**
 * @brief Restarts all tasks.
 * @return RET_OK on success, RET_ERROR on failure.
 */
RetVal restartAllTasks();

/**
 * @brief Retrieves the restart queue handle.
 * @param restartQueueHandle Pointer to store the restart queue handle.
 * @return RET_OK on success, RET_ERROR on failure.
 */
RetVal getRestartQueue(QueueHandle_t restartQueueHandle);

/**
 * @brief Sets task handlers for the slave system.
 * @param taskHandlers Array of task handlers.
 */
void setTaskHandlers(TaskHandle_t *taskHandlers);

#endif // SLAVE_RESTART_THREADS_H