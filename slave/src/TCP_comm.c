#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "slave_state_machine.h"

void tcpEchoServerTask() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024] = {0};
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

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any IP address
    server_addr.sin_port = htons(5001); // Listening port

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

    printf("Server listening on port 5001...\n");

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
                bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            } while (bytes_received < 0 && errno == EINTR);

            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Received: %s\n", buffer);
                uint8_t input = atoi(buffer);
                handelStatus(input);

                // Echo back to client
                send(client_fd, buffer, bytes_received, 0);
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
