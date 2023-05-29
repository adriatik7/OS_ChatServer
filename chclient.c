#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *socket_ptr) {
    int socket = *(int *)socket_ptr;
    char message[BUFFER_SIZE];
    ssize_t bytes_received;
    
    while ((bytes_received = recv(socket, message, sizeof(message), 0)) > 0) {
        message[bytes_received] = '\0';
        printf("%s\n", message);
    }
    
    pthread_exit(NULL);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    
    // Create client socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set server address configuration
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to the chat server.\n");
    
    // Get client name
    char client_name[50];
    printf("Enter your name: ");
    fgets(client_name, sizeof(client_name), stdin);
    client_name[strcspn(client_name, "\n")] = '\0';
    send(client_socket, client_name, strlen(client_name), 0);
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, receive_messages, (void *)&client_socket) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }
    
    // Read and send user messages
    char message[BUFFER_SIZE];
    
    while (fgets(message, sizeof(message), stdin) != NULL) {
        send(client_socket, message, strlen(message), 0);
    }
    
    close(client_socket);
    
    pthread_join(thread, NULL);
    
    return 0;
}
