#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "slave_tcp_comm_client.h"
#include "types.h"

void tcpClientTask(void *pvParameters) {
    TCPConfig *config = (TCPConfig *)pvParameters;

    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[config->buffer_size];
    const char *message = "Hello from FreeRTOS Client!";

    printf("Client Configuration: IP=%s, PORT=%d, BUFFER=%d\n",
           config->server_ip, config->server_port, config->buffer_size);

    while (1) {
        // Create socket
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            perror("Socket creation failed");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config->server_port);
        server_addr.sin_addr.s_addr = inet_addr(config->server_ip);

        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connect failed");
            close(client_fd);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        printf("Connected to server at %s:%d\n", config->server_ip, config->server_port);

        while (1) {
            SlaveStatesMsg receivedData;

            // Wait for data from the queue
            if (xQueueReceive(config->xDataQueue, &receivedData, pdMS_TO_TICKS(5000)) == pdTRUE) {
                // char payload[256];
                // snprintf(payload, sizeof(payload), "status = %d", receivedData);

                // send(client_fd, payload, strlen(payload), 0);
                // printf("[Client] Sent to server: %s\n", payload);

                // // Optional: Receive server response
                // ssize_t bytes_received = recv(client_fd, buffer, config->buffer_size - 1, 0);
                // if (bytes_received > 0) {
                //     buffer[bytes_received] = '\0';
                //     printf("[Client] Server Response: %s\n", buffer);
                // }
            } else {
                // printf("[Client] No data in queue (Timeout)\n");
            }

            vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before next queue check
        }

        // close(client_fd);
        printf("Disconnected from server.\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelete(NULL);
}

