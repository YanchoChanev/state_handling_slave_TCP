#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
    FALSE,
    TRUE
} Bool;

/**
 * @brief Structure defining a queue message.
 */
typedef struct {
    uint8_t msg_id; ///< Message identifier.
    uint8_t state;  ///< State associated with the message.
} QueueMessage;

/**
 * @brief Enumeration of message types.
 */
typedef enum {
    HANDEL_SLAVE_FAULT,  ///< Handle slave fault.
    RESET_SLAVE,         ///< Reset slave.
    SLAVE_CURRENT_STATUS ///< Current status of the slave.
} MessageType;

/**
 * @brief Enumeration of master states.
 */
typedef enum {
    IDLE,            ///< Idle state.
    PROCESSING,      ///< Processing state.
    ERROR,           ///< Error state.
    MAX_STATE_MASTER ///< Maximum state value for master.
} MasterStates;

/**
 * @brief Enumeration of slave states.
 */
typedef enum {
    SLEEP,          ///< Sleep state.
    ACTIVE,         ///< Active state.
    FAULT,          ///< Fault state.
    MAX_STATE_SLAVE ///< Maximum state value for slave.
} SlaveStates;

typedef struct {
    SlaveStates status;
    Bool isNew;
} SlaveStatesMsg;



/**
 * @brief Enumeration of return values.
 */
typedef enum {
    RET_OK,    ///< Operation succeeded.
    RET_ERROR  ///< Operation failed.
} RetVal;

typedef struct {
    char server_ip[16];  // IPv4 Address (e.g., "127.0.0.1")
    uint16_t server_port;
    uint16_t buffer_size;
    QueueHandle_t xDataQueue;
    uint16_t id;
    uint16_t data;
    char buffer[1024];
} TCPConfig;

#define MASTER_SERVER_IP "127.0.0.2"
#define MASTER_SERVER_PORT 5004
#define MASTER_SERVER_SIZE 1024

#define SLAVE_SERVER_IP "127.0.0.1"
#define SLAVE_SERVER_SIMULATION_PORT 5003
#define SLAVE_SERVER_PORT 5002
#define SLAVE_SERVER_SIZE 1024

// Task prioritis
#define TASTK_PRIO_SLAVE_COMM_HANDLER                2 
#define TASTK_PRIO_SLAVE_TEST_TASK_HANDLER           1 
#define TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING 1
#define TASTK_PRIO_SLAVE_RESTAT_STATUS               3
#define TASTK_PRIO_ECHO_SERVER_HANDLER               2
#define TASTK_PRIO_TCP_SERVER_HANDLER                2
#define TASTK_PRIO_TCP_SERVER_SIMULATION_HANDLER     2
#define TASTK_PRIO_TCP_CLIENT_HANDLER                2

#endif // TYPES_H