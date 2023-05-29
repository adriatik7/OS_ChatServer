#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5050
#define BUFFER_SIZE 2048

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t num_bytes;

    // Receive initial server message
    num_bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (num_bytes <= 0) {
        perror("Receive failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    buffer[num_bytes] = '\0';
    printf("Received from server: %s\n", buffer);

    // Send username to the server
    char username[] = "Client1";
    if (send(sock, username, strlen(username), 0) == -1) {
        perror("Send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Read user input from stdin
        printf("Enter a message: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Input error");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send message to the server
        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Receive and print server response
        num_bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (num_bytes <= 0) {
            perror("Receive failed");
            close(sock);
            exit(EXIT_FAILURE);
        }
        buffer[num_bytes] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    // Close the socket
    close(sock);

    return 0;
}

