#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "logger.h"
#include "slave_state_machine.h"
#include "types.h"

// Queue handle for state signals.
static QueueHandle_t stateQueueHandle_ = NULL;

// Current state of the slave.
static SlaveStatesMsg currentState_ = {SLEEP, FALSE};

// Semaphore for state synchronization.
static SemaphoreHandle_t statusSemaphore = NULL;

// Type definition for state handler functions.
typedef RetVal (*StateHandler)();

// Function prototypes for state handlers.
static RetVal handleSleepState();
static RetVal handleActiveState();
static RetVal handleFaultState();

// Function prototype for changing state.
static RetVal changeState(SlaveStates state);

#define SLAVE_MAX_STATE_SLAVE (MAX_STATE_SLAVE + 1)

// State handler function array.
static StateHandler state_handlers[SLAVE_MAX_STATE_SLAVE] = {
    handleSleepState,
    handleActiveState,
    handleFaultState,
    NULL
};

/**
 * @brief Changes the current state of the slave.
 * @param state The new state to transition to.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal changeState(SlaveStates state) {
    if (statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "statusSemaphore is NULL");
        return RET_ERROR;
    }

    if (xSemaphoreTake(statusSemaphore, portMAX_DELAY) == pdTRUE) {
        if (currentState_.status != state) {
            currentState_.status = state;
            currentState_.isNew = TRUE;
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveStateMachine", "New state is %d, isSent = %d", 
                                currentState_.status, currentState_.isNew);
        }
        xSemaphoreGive(statusSemaphore);
        return RET_OK;
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to take status semaphore");
        return RET_ERROR;
    }
}

/**
 * @brief Handles the FAULT state logic.
 */
static RetVal handleFaultState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling FAULT state");
    QueueMessage data = {HANDEL_SLAVE_FAULT, ERROR};

    // if (sendMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
    //     logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Problem sending message over the queue");
    // }
}

/**
 * @brief Handles the SLEEP state logic.
 */
static RetVal handleSleepState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling SLEEP state");
}

/**
 * @brief Handles the ACTIVE state logic.
 */
static RetVal handleActiveState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling ACTIVE state");
}

// Initializes the semaphore used for state synchronization.
RetVal initStateSemaphoreSlave() {
    statusSemaphore = xSemaphoreCreateMutex();
    if (statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to create status semaphore");
        return RET_ERROR;
    }
    return RET_OK;
}

// Handles a change in the slave's status/state.
RetVal handelStatus(SlaveStates state) {

    if (state >= SLAVE_MAX_STATE_SLAVE) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Invalid state");
        return RET_ERROR;
    }

    if (changeState(state) == RET_ERROR) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "State change failed");
        return RET_ERROR;
    }

    // Call the appropriate state handler function
    if (state_handlers[state] != NULL) {
        state_handlers[state]();
    } else {
        logMessageFormatted(LOG_LEVEL_ERROR, "SlaveStateMachine", "No handler for state: %d", state);
        return RET_ERROR;
    }

    return RET_OK;
}

// Retrieves the current state of the slave.
SlaveStatesMsg* getState() {
    return &currentState_;
}

/**
 * @brief Retrieves the current state of the slave.
 * @return The current state.
 */
RetVal statuSent(Bool isSent){
    currentState_.isNew = isSent;
    return RET_OK;
}