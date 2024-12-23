#include "FreeRTOS.h"
#include "queue.h"
#include "slave_restart_threads.h"
#include "slave_handler.h"
#include "logger.h"

// Queue handle for restart signals.
static QueueHandle_t restartQueueHandle_ = NULL;

// Task handlers array.
static TaskHandler taskHandlers_[TAKS_HADLERS_SIZE] = {
    // {0, vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", 
    // TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, NULL},
    {1, vSlaveTaskTestHandling, "TestHandling", TASTK_PRIO_SLAVE_TEST_TASK_HANDLER, NULL},
    // {2, vSlaveCommHandler, "CommHandler", TASTK_PRIO_SLAVE_COMM_HANDLER, NULL},
    // {3, vTCPEchoServerTask, "TCPEchoServerTask", TASTK_PRIO_ECHO_SERVER_HANDLER, NULL},
};

/**
 * @brief Internal function for deletes all tasks in the task handler array.
 */
static void deleteAllTasks() {
    for (uint8_t i = 0; i < TAKS_HADLERS_SIZE; i++) {
        if (taskHandlers_[i].taskHandler != NULL) {
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Deleting task %d", i);
            vTaskDelete(taskHandlers_[i].taskHandler);
            taskHandlers_[i].taskHandler = NULL;
        }
    }
}

/**
 * @brief Internal function for recreates all tasks in the task handler array.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal recreateAllTasks() {
    for (uint8_t i = 0; i < TAKS_HADLERS_SIZE; i++) {
        logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Recreating task %d", i);
        if (xTaskCreate(taskHandlers_[i].taskFunction, taskHandlers_[i].taskName,
                        configMINIMAL_STACK_SIZE, NULL, taskHandlers_[i].taskPrio, 
                        &taskHandlers_[i].taskHandler) != pdPASS) {
            logMessageFormatted(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to create task %d", i);
            return RET_ERROR;
        } else {
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Task %d created successfully", i);
        }
    }
    return RET_OK;
}

// Restarts all tasks.
RetVal restartAllTasks() {
    logMessage(LOG_LEVEL_INFO, "SlaveRestartThread", "Restarting all tasks");
    deleteAllTasks();

    if (recreateAllTasks() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to recreate tasks");
        return RET_ERROR;
    }

    if (handelStatus(SLEEP) != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to set state ACTIVE");
        return RET_ERROR;
    }
    return RET_OK;
}

// Sets task handlers for the slave system.
void setTaskHandlers(TaskHandle_t *taskHandlers) {
    for (int i = 0; i < TAKS_HADLERS_SIZE; i++) {
        taskHandlers_[i].taskHandler = taskHandlers[i];
    }
}
