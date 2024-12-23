#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "slave_handler.h"
#include "TCP_comm.h"
#include "slave_state_machine.h"
#include "queue.h"

// Handles restart signals.
void vRestartHandler(void *args) {
    uint8_t signal;
    

    while (1) {
        // if (reciveMsgSlave(REST_CHANNEL, &signal) == RET_OK) {
        //     logMessage(LOG_LEVEL_DEBUG, "SlaveHandler", "Received restart signal");
        //     if (signal == 1) {
        //         handelStatus(SLEEP);
        //         vTaskDelay(pdMS_TO_TICKS(10000));
        //         if (restartAllTasks() != RET_OK) {
        //             logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to restart all tasks");
        //         }
        //     }
        // }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Observes and retrieves the slave's status.
// void vSlaveStatusObservationHandler(void *args) {
//     logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vSlaveStatusObservationHandler created");
//     QueueMessage data;

//     while (1) {
//         if (reciveMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
//             logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to receive message");
//         } else {
//             logMessageFormatted(LOG_LEVEL_DEBUG, "SlaveHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
//             data.msg_id = SLAVE_CURRENT_STATUS;
//             data.state = getState();
//             // printf("State: %d\n", data.state);
//             if (sendMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
//                 logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send message");
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

// Handles communication tasks.
// void vSlaveCommHandler(void *args) {
//     logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vSlaveCommHandler created");
//     QueueMessage data;

//     while (1) {
//         if (reciveMsgSlave(STATE_CHANNEL, &data) == RET_OK) {
//             logMessageFormatted(LOG_LEVEL_DEBUG, "SlaveHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
//             if (data.msg_id == RESET_SLAVE) {
//                 handelStatus(SLEEP);
//                 uint8_t signal = 1;
//                 if (sendMsgSlave(REST_CHANNEL, &signal) != pdPASS) { // Send restart signal
//                     logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send restart signal");
//                 }
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

// Handles test tasks with user input.
void vSlaveTaskTestHandling(void *args) {
    char input[16];
    int number;
    logMessage(LOG_LEVEL_DEBUG, "SlaveHandler", "vSlaveTaskTestHandling created");

    while (1) {
        printf("Enter a number between 0 and 2: ");
        fflush(stdout);

        while (1) {
            if (fgets(input, sizeof(input), stdin) != NULL) {
                // Remove newline character if present
                input[strcspn(input, "\n")] = 0;

                // Convert input to an integer
                number = atoi(input);

                // Validate the number
                if (number >= 0 && number <= 2) {
                    logMessageFormatted(LOG_LEVEL_INFO, "SlaveHandler", "Valid input received: %d", number);
                    handelStatus(number);
                    break; // Exit the inner loop on valid input
                } else {
                    logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Invalid input. Please enter a number between 0 and 2.");
                }
            } else {
                // logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Error reading input.");
            }

            // Clear the input buffer
            while (getchar() != '\n' && getchar() != EOF);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        // Simulate task delay to prevent tight looping
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// void vTCPEchoServerTask(void *args) {
//     logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vTCPEchoServerTask started");
//     tcpEchoServerTask();
//     vTaskDelay(pdMS_TO_TICKS(100));
// }


void vCreateTCPServer(void *args){
    TCPConfig *configServer = pvPortMalloc(sizeof(TCPConfig));
    if (configServer == NULL) {
        perror("Failed to allocate memory for server config");
        return;
    }

    strncpy(configServer->server_ip, SLAVE_SERVER_IP, sizeof(configServer->server_ip) - 1);
    configServer->server_ip[sizeof(configServer->server_ip) - 1] = '\0';
    configServer->server_port = SLAVE_SERVER_PORT;
    configServer->buffer_size = SLAVE_SERVER_SIZE;

    tcpServerTask((void *)configServer);
}

void vCreateTCPServerSimulation(void *args){
    TCPConfig *configServer = pvPortMalloc(sizeof(TCPConfig));
    if (configServer == NULL) {
        perror("Failed to allocate memory for server config");
        return;
    }

    strncpy(configServer->server_ip, SLAVE_SERVER_IP, sizeof(configServer->server_ip) - 1);
    configServer->server_ip[sizeof(configServer->server_ip) - 1] = '\0';
    configServer->server_port = SLAVE_SERVER_SIMULATION_PORT;
    configServer->buffer_size = SLAVE_SERVER_SIZE;
    configServer->xDataQueue = (QueueHandle_t)args;

    tcpServerTask((void *)configServer);
}

void vCreateTCPClient(void *args){
    TCPConfig *configClient = pvPortMalloc(sizeof(TCPConfig));
    if (configClient == NULL) {
        perror("Failed to allocate memory for client config");
        return;
    }

    strncpy(configClient->server_ip, MASTER_SERVER_IP, sizeof(configClient->server_ip) - 1);
    configClient->server_ip[sizeof(configClient->server_ip) - 1] = '\0';
    configClient->server_port = MASTER_SERVER_PORT;
    configClient->buffer_size = MASTER_SERVER_SIZE;
    configClient->xDataQueue = (QueueHandle_t)args;

    tcpClientTask((void *)configClient);
}