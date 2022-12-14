#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <pthread.h>


// define for UDS
#define SOCK_PATH "tpf_unix_sock.server"
#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"

#define MAXLINE 1024
#define PORT 8080

char globalBuf[MAXLINE]; // shared mem
pthread_mutex_t mutex;


char *IP = "127.0.0.1";
clock_t start;
clock_t end;
const int BUFFER_SIZE = MAXLINE * MAXLINE * 100; // 100 MB
char *fileName = "f.txt";

int create100MBfile()
{
    // Create a buffer to hold the data

    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
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

// #define size 4096 

// int cs_sum(char arr[size], int n) {
//     long long checksum;
//     int sum = 0, i;
//     for (i = 0; i < n; i++)
//         sum += arr[i];
//     checksum = ~sum;    //1's complement of cs_sum
//     return checksum;
// }
int checkSum(char *file_name2)
{

    // int fd2 = open(file_name2, O_CREAT | O_RDWR);
    // int fd1 = open(fileName, O_CREAT | O_RDWR);
//     // if we had problem to open the files.
//     if (fd1 == -1)
//     {
//         perror("open files");
//     }

//     size_t r;
//     long long sch;
//     char buff[size];
//     int sum = 0;
//     while ((r = read(fd1, buff, sizeof(buff))) > 0) {
//         sch = cs_sum(buff, r);
//         bzero(buff, size);
//         sum += sch;
//     }

//     // if we had problem to open the files.
//     if (fd2 == -1) {
//         perror("open files");
//     }

//     size_t r2;
//     char buff2[size];

//     int sum2 = 0;
//     int sch2;
//     while ((r2 = read(fd2, buff2, sizeof(buff2))) > 0) {
//         sch2 = cs_sum(buff2, r2);
//         bzero(buff2, size);
//         sum2 += sch2;
//     }

//     sum2 += ~sum;
//     long long res = ~sum2;
// //    printf("CHECKSUM IS:%lld\n",res);
//     if (res == 0) {
//         return 1;
//     } else {
//         return -1;
//     }
// }

    int f2 = open(file_name2, O_CREAT | O_RDWR);
    int f1 = open(fileName, O_CREAT | O_RDWR);
    // if we had problem to open the files.
    if (f1 == -1)
    {
        perror("open files");
    }
    size_t r;
    long long tmp_sum1;
    char buff[MAXLINE];
    int sum1 = 0;
    while ((r = read(f1, buff, sizeof(buff))) > 0)
    {
        tmp_sum1 = 0;
        for (int i = 0; i < r; i++)
            tmp_sum1 += buff[i];
        bzero(buff, MAXLINE);
        sum1 += tmp_sum1;
    }

    // if we had problem to open the files.
    if (f2 == -1)
    {
        perror("open");
    }

    size_t r2;
    char buff2[MAXLINE];

    int sum2 = 0;
    int tmp_sum2;
    while ((r2 = read(f2, buff2, sizeof(buff2))) > 0)
    {
        tmp_sum2 = 0;
        for (int i = 0; i < r2; i++)
            tmp_sum2 += buff2[i];
        bzero(buff2, MAXLINE);
        sum2 += tmp_sum2;
    }
    close(f1);
    close(f2);

    if (sum2 == sum1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/* TCP */
int senderTCP()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Set the address and port of the remote host.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the remote host.
    int con = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (con == -1)
    {
        perror("connect");
        close(sockfd);
        exit(1);
    }
    // Open the file that you want to send.
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        perror("fopen sender");
        return -1;
    }

    // Read the contents of the file and send it over the socket.
    char buffer[MAXLINE];
    size_t bytes_read;
    start = clock();
    printf("TCP/IPv4 Socket - start: %f\n", (float)start/CLOCKS_PER_SEC);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        send(sockfd, buffer, bytes_read, 0);
        bzero(buffer, MAXLINE);
    }
    // Close the file and the socket.
    fclose(fp);
    close(sockfd);
    return 0;
}

int reciverTCP()
{
    // Create a socket.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to a local address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
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
    // receive data on the socket
    char buf[MAXLINE];
    size_t num_bytes_received;
    size_t num_bytes_written;
    while ((num_bytes_received = recv(client_fd, buf, sizeof(buf), 0)) > 0)
    {
        num_bytes_written = fwrite(buf, sizeof(char), num_bytes_received, file);
        bzero(buf, MAXLINE);
    }

    if (num_bytes_received == -1)
    {
        perror("recive");
        return -1;
    }
    close(sock_fd);
    // close(client_fd);
    fclose(file);

    end = clock();
    int c = checkSum("rec_file.txt");
    if (c == 1)
    {
        printf("TCP/IPv4 Socket - end: %f\n", (double)end/CLOCKS_PER_SEC);
    }
    else if (c == -1)
    {
        printf("TCP/IPv4 Socket - end: -1\n");
    }
    return 0;
}

int sendTCP()
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        senderTCP();
        exit(0);
    }
    else
    {
        reciverTCP();
        wait(NULL);
        
    }
}

/* UDP */
int reciverUDP()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(buffer, 0, MAXLINE);
    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(12345);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen("rec_file_udp.txt", "wb");
    if (file == NULL)
    {
        perror("reciver file");
        return -1;
    }

    int len = sizeof(cliaddr); // len is value/result
    size_t num_bytes_received;
    size_t num_bytes_written;
    while ((num_bytes_received = recvfrom(sockfd, (char *)buffer, MAXLINE,
                                          MSG_WAITALL, (struct sockaddr *)&cliaddr,
                                          &len)) > 0)
    {
        if (num_bytes_received == -1)
        {
            perror("recive");
            return -1;
        }
        // printf("rec: %ld", num_bytes_received);
        num_bytes_written = fwrite(buffer, sizeof(char), num_bytes_received, file);
        // printf("write: %ld\n", num_bytes_written);
        bzero(buffer, MAXLINE);
    }

    fclose(file);
    end = clock();
    int c = checkSum("rec_file_udp.txt");
    if (c == 1)
    {
        printf("UDP/IPv6 Socket - end: %f\n", (double)end/CLOCKS_PER_SEC);
    }
    else if (c == -1)
    {
        printf("UDP/IPv6 Socket - end: -1\n");
    }
    close(sockfd);
    return 0;
}

int senderUDP()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(buffer, 0, MAXLINE);
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Open the file that you want to send.
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        perror("fopen sender");
        return -1;
    }

    // Read the contents of the file and send it over the socket.
    size_t bytes_read;
    start = clock();
    printf("UDP/IPv6 Socket - start: %f\n", (double)start/CLOCKS_PER_SEC);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        size_t bytes_sent = 0;
        while (bytes_sent != bytes_read)
        {
            size_t ret = sendto(sockfd, (const char *)buffer, bytes_read, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
            if(ret > 0){
                bytes_sent += ret;
            }else if (ret < 0)
            {
                perror("send");
                exit(1);
            }
            
        }
        bzero(buffer, MAXLINE);
        
    }
    sendto(sockfd, "", 0, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    // Close the file and the socket.
    fclose(fp);
    close(sockfd);
    return 0;
}

int sendUDP()
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        senderUDP();
        exit(0);
    }
    else
    {
        reciverUDP();
        wait(NULL);
    }
}

/* UDS strem */
int reciverUDS_stream()
{
    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buf[MAXLINE];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, MAXLINE);

    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("SOCKET ERROR");
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCK_PATH);
    len = sizeof(server_sockaddr);

    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        printf("BIND ERROR");
        close(server_sock);
        exit(1);
    }

    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
    rc = listen(server_sock, backlog);
    if (rc == -1)
    {
        printf("LISTEN ERROR");
        close(server_sock);
        exit(1);
    }
    // printf("socket listening...\n");

    /*********************************/
    /* Accept an incoming connection */
    /*********************************/
    client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len);
    if (client_sock == -1)
    {
        printf("ACCEPT ERROR");
        close(server_sock);
        close(client_sock);
        exit(1);
    }

    /****************************************/
    /* Get the name of the connected socket */
    /****************************************/
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *)&client_sockaddr, &len);
    if (rc == -1)
    {
        printf("GETPEERNAME ERROR");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else
    {
    }

    /************************************/
    /* Read and print the data          */
    /* incoming on the connected socket */
    /************************************/

    FILE *file = fopen("rec_file_uds.txt", "wb");
    if (file == NULL)
    {
        perror("reciver file");
        return -1;
    }
    // receive data on the socket
    size_t num_bytes_received;
    size_t num_bytes_written;
    while ((num_bytes_received = recv(client_sock, buf, sizeof(buf), 0)) > 0)
    {
        num_bytes_written = fwrite(buf, sizeof(char), num_bytes_received, file);
        bzero(buf, MAXLINE);
    }

    if (num_bytes_received == -1)
    {
        perror("recive");
        return -1;
    }
    fclose(file);

    end = clock();
    int c = checkSum("rec_file_uds.txt");
    if (c == 1)
    {
        printf("UDS - Stream socket - end: %f\n", (double)end/CLOCKS_PER_SEC);
    }
    else if (c == -1)
    {
        printf("UDS - Stream socket - end: -1\n");
    }
    close(server_sock);
    close(client_sock);
    return 0;
}

int senderUDS_stream()
{
    int client_sock, rc, len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buf[MAXLINE];
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, MAXLINE);
    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1)
    {
        printf("SOCKET ERROR");
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, CLIENT_PATH);
    len = sizeof(client_sockaddr);

    unlink(CLIENT_PATH);
    rc = bind(client_sock, (struct sockaddr *)&client_sockaddr, len);
    if (rc == -1)
    {
        printf("BIND ERROR");
        close(client_sock);
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* for the server socket and connect   */
    /* to it.                              */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SERVER_PATH);
    rc = connect(client_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        printf("CONNECT ERROR");
        close(client_sock);
        exit(1);
    }

    // Open the file that you want to send.
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        perror("fopen sender");
        return -1;
    }

    // Read the contents of the file and send it over the socket.
    size_t bytes_read;
    start = clock();
    printf("UDS - Stream socket - start: %f\n", (double)start/CLOCKS_PER_SEC);
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        send(client_sock, buf, bytes_read, 0);
        bzero(buf, MAXLINE);
    }
    // Close the file and the socket.
    fclose(fp);
    close(client_sock);

    return 0;
}

int sendUDS_stream()
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        senderUDS_stream();
        exit(0);
    }
    else
    {
        reciverUDS_stream();
        wait(NULL);
    }
}

/* UDS datagram */
int reciverUDS_datagram()
{
    int server_sock, len, rc;
    struct sockaddr_un server_sockaddr, peer_sock;
    char buf[MAXLINE];
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, MAXLINE);

    /****************************************/
    /* Create a UNIX domain datagram socket */
    /****************************************/
    server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_sock == -1)
    {
        printf("SOCKET ERROR");
        exit(1);
    }
    else
    {
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCK_PATH);
    len = sizeof(server_sockaddr);
    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        printf("BIND ERROR");
        close(server_sock);
        exit(1);
    }

    FILE *file = fopen("rec_file_uds_dg.txt", "wb");
    if (file == NULL)
    {
        perror("reciver file");
        return -1;
    }

    /****************************************/
    /* Read data on the server from clients */
    /* and print the data that was read.    */
    /****************************************/
    size_t bytes_rec = 0;

    while ((bytes_rec = recvfrom(server_sock, (char *)buf, sizeof(buf), MSG_WAITALL, (struct sockaddr *)&server_sockaddr, &len)) > 0)
    {
        if (bytes_rec == -1)
        {
            printf("RECVFROM ERROR");
            close(server_sock);
            exit(1);
        }
        else
        {
            fwrite(buf, sizeof(char), bytes_rec, file);
            bzero(buf, MAXLINE);
        }
    }
    fclose(file);
    end = clock();
    int c = checkSum("rec_file_uds_dg.txt");
    if (c == 1)
    {
        printf("UDS - Dgram socket - end: %f\n", (double)end/CLOCKS_PER_SEC);
    }
    else if (c == -1)
    {
        printf("UDS - Dgram socket - end: -1\n");
    }

    close(server_sock);

    return 0;
}

int senderUDS_datagram()
{
    int client_sock, rc;
    struct sockaddr_un remote;
    char buf[MAXLINE];
    memset(&remote, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, MAXLINE);
    /****************************************/
    /* Create a UNIX domain datagram socket */
    /****************************************/
    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock == -1)
    {
        printf("SOCKET ERROR");
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to send to.    */
    /***************************************/
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SERVER_PATH);

    /***************************************/
    /* Copy the data to be sent to the     */
    /* buffer and send it to the server.   */
    /***************************************/

    // Open the file that you want to send.
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        perror("fopen sender");
        return -1;
    }

    size_t bytes_read;
    start = clock();
    printf("UDS - Dgram socket - start: %f\n", (double)start/CLOCKS_PER_SEC);
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        sendto(client_sock, (const char *)buf, bytes_read, MSG_CONFIRM, (const struct sockaddr *)&remote, sizeof(remote));
        if (rc == -1)
        {
            printf("SENDTO ERROR\n");
            close(client_sock);
            exit(1);
        }
        else
        {
            bzero(buf, MAXLINE);
        }
    }
    sendto(client_sock, "", 0, 0, (struct sockaddr *)&remote, sizeof(remote));

    /*****************************/
    /* Close the socket and exit */
    /*****************************/
    fclose(fp);
    close(client_sock);

    return 0;
}

int sendUDS_datagram()
{

    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        senderUDS_datagram();
        exit(0);
    }
    else
    {
        reciverUDS_datagram();
        wait(NULL);
    }
}

int myMmap()
{

    int fd = open(fileName, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    // Get the size of the file
    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;

    // Map the file to memory
    start = clock();
    printf("MMAP - start: %f\n", (double)start/CLOCKS_PER_SEC);
    void *addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    FILE *file = fopen("rec_file_mmap.txt", "wb");
    if (file == NULL)
    {
        perror("reciver file");
        return -1;
    }
    for (size_t i = 0; i < filesize; i++)
    {
        fwrite(&(*((char *)addr)), 1, sizeof(char), file);
        addr++;
    }

    fclose(file);
    end = clock();
    int c = checkSum("rec_file_mmap.txt");
    if (c == 1)
    {
        printf("MMAP - end: %f\n", (double)end/CLOCKS_PER_SEC);
    }
    else if (c == -1)
    {
        printf("MMAP - end: -1\n");
    }

    return 0;
}

int myPipe()
{
    int filedes[2];
    pid_t childpid;

    pipe(filedes);

    if ((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        close(filedes[0]); // Child process does not need this end of the pipe

        /* Send "string" through the output side of pipe */
        char buf[MAXLINE];
        FILE *fp = fopen(fileName, "rb");
        if (!fp)
        {
            perror("fopen sender");
            return -1;
        }

        size_t bytes_read;
        start = clock();
        printf("PIPE - start: %f\n", (double)start/CLOCKS_PER_SEC);
        while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0)
        {
            write(filedes[1], buf, bytes_read);
        }
        close(filedes[1]);
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        close(filedes[1]); // Parent process does not need this end of the pipe

        FILE *file = fopen("rec_file_pipe.txt", "wb");
        if (file == NULL)
        {
            perror("reciver file");
            return -1;
        }
        char readbuffer[MAXLINE];
        size_t num_bytes_received;
        /* Read in a string from the pipe */
        while (num_bytes_received = read(filedes[0], readbuffer, sizeof(readbuffer)))
        {
            
            fwrite(readbuffer, sizeof(char), num_bytes_received, file);
            bzero(readbuffer, MAXLINE);
        }
        close(filedes[0]);
        fclose(file);

        end = clock();
        int c = checkSum("rec_file_pipe.txt");
        if (c == 1)
        {
            printf("PIPE - end: %f\n", (double)end/CLOCKS_PER_SEC);
        }
        else if (c == -1)
        {
            printf("PIPE - end: -1\n");
        }
    }

    return 0;
}
// Thread 1: read from the file and write to the buffer
void *thread_1(void *arg)
{
    // Open the file for reading
    FILE *file = fopen(fileName, "r");

    // Read from the file and write to the buffer
    char tmpBuf[MAXLINE];
    while (!feof(file))
    {
        // Read a chunk of data from the file
        pthread_mutex_lock(&mutex);
        int bytes_read = fread(tmpBuf, 1, sizeof(tmpBuf), file);

        // Write the data to the buffer
        for (int i = 0; i < bytes_read; i++)
        {
            globalBuf[i] = tmpBuf[i];
        }
        bzero(tmpBuf, MAXLINE);
        pthread_mutex_unlock(&mutex);
    }

    // Close the file
    fclose(file);

    // Return from the thread
    pthread_exit(NULL);
}

// Thread 2: read from the buffer and write to a new file
void *thread_2(void *arg)
{
    // Open the new file for writing
    FILE *file = fopen("new_file.txt", "w");

    // Read from the buffer and write to the new file
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < sizeof(globalBuf); i++)
    {
        fputc(globalBuf[i], file);
    }
    bzero(globalBuf, MAXLINE);
    pthread_mutex_unlock(&mutex);

    // Close the new file
    fclose(file);

    // Return from the thread
    pthread_exit(NULL);
}

int sharedMemory()
{
    // Create the two threads
    pthread_mutex_init(&mutex, NULL);
    pthread_t thread_1_id, thread_2_id;
    pthread_create(&thread_1_id, NULL, thread_1, NULL);
    pthread_create(&thread_2_id, NULL, thread_2, NULL);
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    return 0;
}

int main(int argc, char *argv[])
{
    create100MBfile();
    sendTCP();
    sendUDS_stream();
    // sendUDP();
    sendUDS_datagram();
    myMmap();
    myPipe();
    return 0;
}
