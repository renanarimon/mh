#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/in.h>

#define MESSAGE_LENGTH 1024

sem_t sem;
int count;

void *send_thread_func(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    char message[MESSAGE_LENGTH];

    for(size_t i =0; i< 1000000; i++)
    {
        printf("Enter a message to send: ");
        fflush(stdout);
        count++;
        printf("count: %d", count);
        fflush(stdout);
        sem_wait(&sem);
        send(sock, message, strlen(message), 0);
    }

    return 0;
}

void *recv_thread_func(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    char message[MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(sock, message, MESSAGE_LENGTH, 0)) > 0)
    {
        // read_size = recv(sock, message, MESSAGE_LENGTH, 0);
        // if (read_size > 0)
        // {
        printf("Received message: %s", message);
        count++;
        sem_post(&sem);
        // }
    }

    return 0;
}

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in server;
    pthread_t send_thread, recv_thread;

    // Initialize the semaphore
    sem_init(&sem, 0, 1);

    // Create the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }

    // Listen
    listen(sock, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    int client_sock = accept(sock, NULL, NULL);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    // Create the send and receive threads
    if (pthread_create(&send_thread, NULL, send_thread_func, &client_sock) < 0)
    {
        perror("could not create send thread");
        return 1;
    }
    if (pthread_create(&recv_thread, NULL, recv_thread_func, &client_sock) < 0)
    {
        perror("could not create recv thread");
        return 1;
    }

    // Wait for the threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    // Clean up
    close(sock);
    sem_destroy(&sem);

    return 0;
}