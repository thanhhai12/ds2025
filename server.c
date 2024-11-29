#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void receive_file(int client_socket) {
    char buffer[BUFFER_SIZE];
    FILE *fp = fopen("received_file.txt", "wb");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, sizeof(char), bytes_received, fp);
    }

    if (bytes_received < 0) {
        perror("Error receiving file");
    }

    printf("File received successfully.\n");
    fclose(fp);
    send(client_socket, "TRANSFER_COMPLETE", 17, 0); // Send acknowledgment
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == 0) {
        printf("Server listening on port %d...\n", PORT);
    } else {
        perror("Listen failed");
        exit(1);
    }

    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket < 0) {
        perror("Server accept failed");
        exit(1);
    }

    printf("Connection established with client.\n");

    // Receive file from client
    receive_file(client_socket);

    close(client_socket);
    close(server_socket);
    return 0;
}
