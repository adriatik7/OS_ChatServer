#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
    char client_name[50];
} Client;

Client clients[MAX_CLIENTS];
pthread_t threads[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message_to_all(const char *message, const char *sender) {
    pthread_mutex_lock(&clients_mutex);
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char formatted_time[9];
    strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S", time_info);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].client_socket != -1 && strcmp(clients[i].client_name, sender) != 0) {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, BUFFER_SIZE, "[%s] %s: %s", formatted_time, sender, message);
            write(clients[i].client_socket, formatted_message, strlen(formatted_message));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_private_message(const char *recipient, const char *message, const char *sender) {
    pthread_mutex_lock(&clients_mutex);
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char formatted_time[9];
    strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S", time_info);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].client_socket != -1 && strcmp(clients[i].client_name, recipient) == 0) {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, BUFFER_SIZE, "[%s] [Private from %s]: %s", formatted_time, sender, message);
            write(clients[i].client_socket, formatted_message, strlen(formatted_message));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_welcome_message(int client_socket) {
    char welcome_message[] = "Welcome to the chat server! You are now connected.\n";
    write(client_socket, welcome_message, strlen(welcome_message));
}

void send_disconnection_message(int client_socket) {
    char disconnection_message[] = "You have been disconnected from the server.\n";
    write(client_socket, disconnection_message, strlen(disconnection_message));
}

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char client_name[50];
    recv(client_socket, client_name, sizeof(client_name), 0);
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].client_socket == -1) {
            clients[i].client_socket = client_socket;
            strncpy(clients[i].client_name, client_name, sizeof(clients[i].client_name));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    send_welcome_message(client_socket);
    printf("Client '%s' connected.\n", client_name);
    
    char message[BUFFER_SIZE];
    ssize_t bytes_received;
    
    while ((bytes_received = recv(client_socket, message, sizeof(message), 0)) > 0) {
        if (bytes_received > 1 && message[0] == '/') {
            if (strncmp(message, "/list", 5) == 0) {
                pthread_mutex_lock(&clients_mutex);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].client_socket != -1) {
                        write(client_socket, clients[i].client_name, strlen(clients[i].client_name));
                        write(client_socket, "\n", 1);
                    }
                }
                pthread_mutex_unlock(&clients_mutex);
            } else if (strncmp(message, "/msg", 4) == 0) {
                char *recipient = strtok(message + 5, " ");
                char *private_message = strtok(NULL, "\n");
                if (recipient && private_message) {
                    send_private_message(recipient, private_message, client_name);
                }
            } else if (strncmp(message, "/quit", 5) == 0) {
                send_disconnection_message(client_socket);
                break;
            } else {
                write(client_socket, "Unknown command\n", 16);
            }
        } else {
            send_message_to_all(message, client_name);
        }
        
        memset(message, 0, sizeof(message));
    }
    
    close(client_socket);
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].client_socket == client_socket) {
            printf("Client '%s' disconnected.\n", clients[i].client_name);
            clients[i].client_socket = -1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);
    
    // Initialize clients array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].client_socket = -1;
    }
    
    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set server address configuration
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(9999);
    
    // Bind server socket to address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for client connections
    if (listen(server_socket, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Chat server is running. Listening on port 8888.\n");
    
    while (1) {
        // Accept a new client connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *)&client_socket) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
        
        // Add thread to the threads array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (threads[i] == 0) {
                threads[i] = thread;
                break;
            }
        }
    }
    
    // Join all client threads
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (threads[i] != 0) {
            pthread_join(threads[i], NULL);
        }
    }
    
    return 0;
}
