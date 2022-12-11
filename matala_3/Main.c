#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unistd.h>

char *IP = "127.0.0.1";
clock_t start;
clock_t end;
const int BUFFER_SIZE = 1024 * 1024 * 100; // 100 MB
char *fileName = "f.txt";
int CHECK_SUM = 0;

int create100MBfile()
{
    // Create a buffer to hold the data

    char *buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer)
    {
        perror("malloc");
        return 1;
    }

    // Fill the buffer with random characters
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = rand() % 256;
    }

    // Open the file
    FILE *file = fopen(fileName, "wb");
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    // Write the data to the file
    size_t bytes_written = fwrite(buffer, sizeof(char), BUFFER_SIZE, file);
    if (bytes_written != BUFFER_SIZE)
    {
        fprintf(stderr, "Error writing to file\n");
        return 1;
    }

    // Close the file
    if (fclose(file) != 0)
    {
        perror("fclose");
        return 1;
    }

    // Free the buffer
    free(buffer);
    return 0;
}

int checkSum(char *file_name)
{
    FILE *fp;
    size_t len;
    char buf[4096];

    if (NULL == (fp = fopen(file_name, "rb")))
    {
        printf("Unable to open %s for reading\n", file_name);
        return -1;
    }
    len = fread(buf, sizeof(char), sizeof(buf), fp);

    size_t i;
    int seed = 0;
    for (i = 0; i < len; ++i)
        seed += (unsigned int)(buf[i]);
    return seed;
}

int sender(int protocol_type)
{
    printf("sender\n");
    fflush(stdout);
    // const int BUFFER_SIZE = 1024 * 1024 * 100; // 100 MB
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Set the address and port of the remote host.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the remote host.
    int con = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if(con == -1){
        perror("connect");
    }
    // Open the file that you want to send.
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        perror("fopen sender");
        return -1;
    }

    // Read the contents of the file and send it over the socket.
    char buffer[1024];
    size_t bytes_read;
    start = clock();
    printf("TCP/IPv4 Socket - start: %ld\n", start);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        send(sockfd, buffer, bytes_read, 0);
        bzero(buffer, 1024);
    }
    // Close the file and the socket.
    fclose(fp);
    close(sockfd);
    return 0;
}

int reciver(int protocol_type)
{
    printf("reciver\n");
    fflush(stdout);

    // Create a socket.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to a local address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return -1;
    }

    // Put the socket into listening mode
    if (listen(sock_fd, 10) == -1)
    {
        perror("listen");
        return -1;
    }

    // Accept incoming connections
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1)
    {
        perror("accept");
        return -1;
    }

    FILE *file = fopen("rec_file.txt", "wb");
    if (file == NULL)
    {
        perror("reciver file");
        return -1;
    }
    // Send and receive data on the socket
    char buf[1024];
    size_t num_bytes_received;
    size_t num_bytes_written;
    while ((num_bytes_received = recv(client_fd, buf, sizeof(buf), 0)) > 0)
    {
        num_bytes_written = fwrite(buf, sizeof(char), num_bytes_received, file);
        bzero(buf, 1024);
    }

    if (num_bytes_received == -1)
    {
        perror("recive");
        return -1;
    }

    end = clock();
    int c = checkSum("rec_file.txt");
    printf("rec: %d, sen: %d\n", c, CHECK_SUM);
    fflush(stdout);
    if (checkSum("rec_file.txt") != CHECK_SUM)
    {
        printf("TCP/IPv4 Socket - end: -1\n");
    }
    else{
        printf("TCP/IPv4 Socket - end: %ld\n", end);
    }
    fclose(file);
    return 0;
}

int sendWithProtocol(int protocol_type)
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        printf("pid 0\n");
        sender(protocol_type);
    }
    else
    {
        printf("father\n");
        reciver(protocol_type);
        wait(NULL);
    }
}

int main(int argc, char *argv[])
{
    create100MBfile();
    CHECK_SUM = checkSum(fileName);
    sendWithProtocol(0);

    return 0;
}
