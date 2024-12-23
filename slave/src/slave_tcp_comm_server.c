#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "slave_tcp_comm_server.h"
#include "types.h"
#include "slave_state_machine.h"
#include "slave_restart_threads.h"

void tcpServerTask(void *pvParameters) {
    TCPConfig *config = (TCPConfig *)pvParameters;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    // char buffer[config->buffer_size];
    int opt = 1;

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        vTaskDelete(NULL);
    }

    // Set socket options to reuse the address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEPORT failed");
        close(server_fd);
        vTaskDelete(NULL);
    }
    uint16_t port = config->server_port;
    printf("------------------------> PORT: %d\n", port);
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any IP address
    server_addr.sin_port = htons(port); // Listening port

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    // Start listening
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    printf("Server listening on port %d...\n", config->server_port);

    for (;;) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            // perror("Accept failed");
            continue;
        }

        printf("Client connected!\n");

        while (1) {
            ssize_t bytes_received;

            do {
                bytes_received = recv(client_fd, config->buffer, sizeof(config->buffer) - 1, 0);
            } while (bytes_received < 0 && errno == EINTR);

            if (bytes_received > 0) {
                config->buffer[bytes_received] = '\0';
                // printf("Received: %s\n", config->buffer);
                if (sscanf(config->buffer, "ID=%d;DATA=%d", &config->id, &config->data) == 2) {
                    // printf("Parsed ID: %d\n", config->id);
                    // printf("Parsed DATA: %d\n", config->data);
                } else {
                    printf("Failed to parse buffer: %s\n", config->buffer);
                }
                uint16_t id = config->id;
                uint16_t data = config->data;

                if(data == ERROR){
                    printf("--------------------->Restarting all tasks\n");
                    restartAllTasks();
                }
                // printf("ID: %d\n", id);
                if(id == SLAVE_SERVER_SIMULATION_PORT){
                    printf("Received: %s\n", config->buffer);
                    printf("Simulation port %d\n", data);
                    handelStatus(data);
                }else if(id == SLAVE_SERVER_PORT){
                    // printf("Simulation port 2\n");
                    SlaveStatesMsg* curentStatus = getState();
                    if(curentStatus->isNew == TRUE){
                        printf("--------------------------------->Status: %d\n", curentStatus->status);
                        statuSent(FALSE);
                        
                        char message[256];
                        snprintf(message, sizeof(message), "STATUS=%d", curentStatus->status);

                        printf("Message: %d\n", data);
                        
                        if (send(client_fd, message, strlen(message), 0) >= 0) {
                            printf("-------------------------------->[Server] Sent to client: %s\n", message);
                        } else {
                            perror("[Server] Failed to send data to client");
                        }

                        
                        
                        // printf("Tyre to send to client\n");
                        // if (xQueueSend(config->xDataQueue, &curentStatus->status, pdMS_TO_TICKS(1000)) == pdTRUE) {
                        //     printf("[Producer] Sent to queue: status=%d\n", curentStatus->status);
                        // } else {
                        //     printf("[Producer] Failed to send data to queue\n");
                        // }
                    }
                }

                // Echo back to client
                send(client_fd, config->buffer, bytes_received, 0);
            } 
            else if (bytes_received == 0) {
                printf("Client disconnected.\n");
                break;
            } 
            else {
                perror("recv failed");
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(80));
        }

        close(client_fd);
        printf("Connection closed.\n");
    }

    close(server_fd);
    vTaskDelete(NULL);
}


