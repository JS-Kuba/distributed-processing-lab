#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// working with pipes
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define HOST "0.0.0.0"
#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 1024

typedef struct {
    int socket_fd;
    int pipe_fd[2]; // Change to an array of two file descriptors
} Client;

Client clients[MAX_CLIENTS];

int client_count = 0;
pthread_mutex_t clients_mutex;

void *handle_client(void *client_ptr);
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
        clients[client_count].socket_fd = client_socket;

        // pipe initialization
        if (pipe(clients[client_count].pipe_fd) == -1) // Change to use the pipe_fd array
        {
            perror("Error creating pipe");
            close(client_socket);
            continue;
        }

        client_count++;

        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&client_thread, NULL, handle_client, (void *)&clients[client_count - 1]) != 0)
        {
            perror("Error creating client thread");
            exit(1);
        }
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *client_ptr)
{
    Client *client = (Client *)client_ptr;
    int client_socket = client->socket_fd;
    int pipe_read_fd = client->pipe_fd[0]; // Read end of the pipe
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(client_socket, &read_fds);
        FD_SET(pipe_read_fd, &read_fds);

        int max_fd =(client_socket > pipe_read_fd) ? client_socket : pipe_read_fd;
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
    {
        perror("Error in select");
        remove_client(client_socket);
        break;
    }

    if (FD_ISSET(client_socket, &read_fds))
    {
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            remove_client(client_socket);
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Received from %d (socket): %s\n", client_socket, buffer);
        fflush(stdout);

        broadcast_message(buffer, client_socket);
        printf("Sent through pipe\n");
        fflush(stdout);
    }
    else if (FD_ISSET(pipe_read_fd, &read_fds))
    {
        bytes_received = read(pipe_read_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_received <= 0)
        {
            perror("Error reading from pipe");
            remove_client(client_socket);
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Received from %d (pipe): %s\n", client_socket, buffer);
        fflush(stdout);

        send(client_socket, buffer, strlen(buffer), 0);
        printf("Sent through socket\n");
        fflush(stdout);
    }
}

return NULL;
}

void broadcast_message(char *message, int sender_socket)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].socket_fd != sender_socket)
        {
            if (write(clients[i].pipe_fd[1], message, strlen(message) + 1) == -1) // Write to the write end of the pipe
            {
                perror("Error writing to pipe");
                remove_client(clients[i].socket_fd);
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
        if (clients[i].socket_fd == client_socket)
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

    close(client_socket);
    printf("Disconnected: %d\n", client_socket);
    fflush(stdout);
}

