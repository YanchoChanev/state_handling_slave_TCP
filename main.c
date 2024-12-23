/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h> // Include pthread.h to use PTHREAD_STACK_MIN

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include the task headers */
#include "slave_handler.h"
#include "slave_restart_threads.h"
#include "types.h"
#include "logger.h"

/* Define the maximum number of messages and the maximum message size */
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(QueueMessage)  

static RetVal initComponents() {
    if(initStateSemaphoreSlave() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Slave faild");
        return RET_ERROR;
    }

    return RET_OK;
} 

static RetVal createStateQueue(){
    // Create the queue. It can hold MAX_MESSAGES number of QueueMessage structures
   

    return RET_OK;
}

static RetVal creatSlaveTasks(){
    TaskHandle_t slaveTaskHandels[TAKS_HADLERS_SIZE] = {NULL, NULL, NULL, NULL};

    QueueHandle_t xDataQueue = xQueueCreate(10, sizeof(SlaveStates));
    if (xDataQueue == NULL) {
        printf("Failed to create the queue\n");
        return 1;
    }

    /* Create the slave task */
    if (xTaskCreate(vSlaveTaskTestHandling, "SlaveTaskTestHandling", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_TEST_TASK_HANDLER, &slaveTaskHandels[TASK_TEST_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vRestartHandler, "RestartSlave", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_RESTAT_STATUS, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vCreateTCPServer, "CreateTCPServer", PTHREAD_STACK_MIN, xDataQueue, 
                    TASTK_PRIO_TCP_SERVER_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vCreateTCPClient, "CreateTCPClient", PTHREAD_STACK_MIN, xDataQueue, 
                    TASTK_PRIO_TCP_CLIENT_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vCreateTCPServerSimulation, "CreateTCPServerSimulation", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_TCP_SERVER_SIMULATION_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    setTaskHandlers(&slaveTaskHandels[0]);
    return RET_OK;
}

int main(void)
{
    if(createStateQueue() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create State Queue faild");
        return 1;
    }
   
    if(initComponents() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Components faild");
        return 1;
    }

    if(creatSlaveTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Slave Tasks faild");
        return 1;
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
    return 0; 
}
