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



char *IP = "127.0.0.1";
#define MAXLINE 1024 
#define PORT     8080 
clock_t start;
clock_t end;
const int BUFFER_SIZE = MAXLINE * MAXLINE * 100; // 100 MB
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

int checkSum(char *file_name2) {
    int f2 = open(file_name2, O_CREAT | O_RDWR);
    int f1 = open(fileName, O_CREAT | O_RDWR);
    // if we had problem to open the files.
    if (f1 == -1) {
        perror("open files");
    }
    size_t r;
    long long tmp_sum1;
    char buff[MAXLINE];
    int sum1 = 0;
    while ((r = read(f1, buff, sizeof(buff))) > 0) {
        tmp_sum1 = 0;
        for (int i = 0; i < r; i++)
            tmp_sum1 += buff[i];
        bzero(buff, MAXLINE);
        sum1 += tmp_sum1;
    }

    // if we had problem to open the files.
    if (f2 == -1) {
        perror("open");
    }

    size_t r2;
    char buff2[MAXLINE];

    int sum2 = 0;
    int tmp_sum2;
    while ((r2 = read(f2, buff2, sizeof(buff2))) > 0) {
        tmp_sum2 = 0;
        for (int i = 0; i < r2; i++)
            tmp_sum2 += buff2[i];
        bzero(buff2, MAXLINE);
        sum2 += tmp_sum2;
    }
    close(f1);
    close(f2);

    if (sum2 == sum1) {
        return 1;
    } else {
        return -1;
    }
}

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
    char buffer[MAXLINE];
    size_t bytes_read;
    start = clock();
    printf("TCP/IPv4 Socket - start: %ld\n", start);
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
    fclose(file);

    end = clock();
    int c = checkSum("rec_file.txt");
    if(c == 1){
        printf("TCP/IPv4 Socket - end: %ld\n", end);
    }
    else if(c == -1){
        printf("TCP/IPv4 Socket - end: %ld\n", end);
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
    }
    else
    {
        reciverTCP();
        wait(NULL);
    }
}



int reciverUDP(){
    int sockfd; 
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr; 

     // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    memset(buffer, 0, MAXLINE);  
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
        
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
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
    // receive data on the socket
    char buf[MAXLINE];
    
    memset(buf, 0, MAXLINE);  
    int len = sizeof(cliaddr);  //len is value/result 
    size_t num_bytes_received;
    size_t num_bytes_written;
    while ((num_bytes_received = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len)) > 0)
    {
        printf("r");
        fflush(stdout);
        num_bytes_written = fwrite(buffer, sizeof(char), num_bytes_received, file);
        bzero(buffer, MAXLINE);
        bzero(buf, MAXLINE);
    }
    printf("\n**************end r****************\n");
    fflush(stdout);
    if (num_bytes_received == -1)
    {
        perror("recive");
        return -1;
    }

    fclose(file);
    end = clock();
    int c = checkSum("rec_file_udp.txt");
    if(c == 1){
        printf("UDP/IPv4 Socket - end: %ld\n", end);
    }
    else if(c == -1){
        printf("UDP/IPv4 Socket - end: %ld\n", end);
    }
    return 0;
}

int senderUDP(){
    int sockfd; 
    char buffer[MAXLINE]; 
    struct sockaddr_in     servaddr; 
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(buffer, 0, MAXLINE); 
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
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
    printf("UDP/IPv4 Socket - start: %ld\n", start);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        printf("s");
        fflush(stdout);
        sendto(sockfd, (const char*)buffer, bytes_read, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        bzero(buffer, MAXLINE);
    }
    printf("\n**************end s****************\n");
    fflush(stdout);
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
    }
    else
    {
        reciverUDP();
        wait(NULL);
    }
}

#define SOCK_PATH  "tpf_unix_sock.server"
int reciverUDS(){
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
    if (server_sock == -1){
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
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc == -1){
        printf("BIND ERROR");
        close(server_sock);
        exit(1);
    }
    
    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
    rc = listen(server_sock, backlog);
    if (rc == -1){ 
        printf("LISTEN ERROR");
        close(server_sock);
        exit(1);
    }
    // printf("socket listening...\n");
    
    /*********************************/
    /* Accept an incoming connection */
    /*********************************/
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock == -1){
        printf("ACCEPT ERROR");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    
    /****************************************/
    /* Get the name of the connected socket */
    /****************************************/
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (rc == -1){
        printf("GETPEERNAME ERROR");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else {
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
    if(c == 1){
        printf("UDS/IPv4 Socket - end: %ld\n", end);
    }
    else if(c == -1){
        printf("UDS/IPv4 Socket - end: %ld\n", end);
    }
    close(server_sock);
    close(client_sock);
    return 0;
}

#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"

int senderUDS(){
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
    if (client_sock == -1) {
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
    rc = bind(client_sock, (struct sockaddr *) &client_sockaddr, len);
    if (rc == -1){
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
    rc = connect(client_sock, (struct sockaddr *) &server_sockaddr, len);
    if(rc == -1){
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
    printf("UDS/IPv4 Socket - start: %ld\n", start);
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        send(client_sock, buf, bytes_read, 0);
        bzero(buf, MAXLINE);
    }
    // Close the file and the socket.
    fclose(fp);
    
    // /************************************/
    // /* Copy the data to the buffer and  */
    // /* send it to the server socket.    */
    // /************************************/
    // strcpy(buf, DATA);                 
    // printf("Sending data...\n");
    // rc = send(client_sock, buf, strlen(buf), 0);
    // if (rc == -1) {
    //     printf("SEND ERROR = %d\n", sock_errno());
    //     close(client_sock);
    //     exit(1);
    // }   
    // else {
    //     printf("Data sent!\n");
    // }

    // /**************************************/
    // /* Read the data sent from the server */
    // /* and print it.                      */
    // /**************************************/
    // printf("Waiting to recieve data...\n");
    // memset(buf, 0, sizeof(buf));
    // rc = recv(client_sock, buf, sizeof(buf));
    // if (rc == -1) {
    //     printf("RECV ERROR = %d\n", sock_errno());
    //     close(client_sock);
    //     exit(1);
    // }   
    // else {
    //     printf("DATA RECEIVED = %s\n", buf);
    // }
    
    /******************************/
    /* Close the socket and exit. */
    /******************************/
    close(client_sock);
    
    return 0;
}

int sendUDS()
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid == 0)
    {
        senderUDS();
    }
    else
    {
        reciverUDS();
        wait(NULL);
    }
}

int main(int argc, char *argv[])
{
    create100MBfile();
    sendUDS();
    return 0;
}
