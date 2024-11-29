#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void send_file(FILE *fp, int sockfd) {
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) == -1) {
            perror("Error sending file");
            exit(1);
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost for testing

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server.\n");

    // Open file to send
    fp = fopen("file_to_send.txt", "rb");
    if (fp == NULL) {
        perror("File not found");
        exit(1);
    }

    // Send file
    send_file(fp, sockfd);
    printf("File sent successfully.\n");

    // Receive acknowledgment from server
    char ack[BUFFER_SIZE];
    recv(sockfd, ack, sizeof(ack), 0);
    printf("Server response: %s\n", ack);

    fclose(fp);
    close(sockfd);
    return 0;
}
