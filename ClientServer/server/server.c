#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "0.0.0.0"
#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 1024

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex;

void *handle_client(void *client_socket_ptr);
void broadcast_message(char *message, int sender_socket);
void remove_client(int client_socket);

int main()
{
    printf("Starting the server.\n");
    fflush(stdout);

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    pthread_t client_thread;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(HOST);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error binding to address");
        exit(1);
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Error listening");
        exit(1);
    }

    printf("Server is running on %s:%d\n", HOST, PORT);
    fflush(stdout);

    pthread_mutex_init(&clients_mutex, NULL);

    while (1)
    {
        client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1)
        {
            perror("Error accepting client");
            continue;
        }

        printf("Connected to %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        fflush(stdout);
        pthread_mutex_lock(&clients_mutex);
        clients[client_count++] = client_socket;
        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&client_thread, NULL, handle_client, (void *)&client_socket) != 0)
        {
            perror("Error creating client thread");
            exit(1);
        }
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *client_socket_ptr)
{
    int client_socket = *((int *)client_socket_ptr);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1)
    {
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            remove_client(client_socket);
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Received from %d: %s\n", client_socket, buffer);
        fflush(stdout);

        broadcast_message(buffer, client_socket);
    }

    return NULL;
}

void broadcast_message(char *message, int sender_socket)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] != sender_socket)
        {
            if (send(clients[i], message, strlen(message), 0) == -1)
            {
                perror("Error sending message");
                remove_client(clients[i]);
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == client_socket)
        {
            for (int j = i; j < client_count - 1; j++)
            {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    pthread_mutex_destroy(&clients_mutex);
    close(client_socket);
    printf("Disconnected: %d\n", client_socket);
    fflush(stdout);
}
