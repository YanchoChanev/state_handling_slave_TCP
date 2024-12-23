#ifndef SLAVE_STATE_MACHINE_H
#define SLAVE_STATE_MACHINE_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Initializes the semaphore used for state synchronization.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal initStateSemaphoreSlave();

/**
 * @brief Handles a change in the slave's status/state.
 * @param state The new state to transition to.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal handelStatus(SlaveStates state);

/**
 * @brief Retrieves the current state of the slave.
 * @return The current state.
 */
SlaveStatesMsg* getState();

/**
 * @brief Retrieves the current state of the slave.
 * @return The current state.
 */
RetVal statuSent(Bool isSent);

#endif // SLAVE_STATE_MACHINE_H