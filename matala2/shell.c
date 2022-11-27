#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <netinet/in.h> //structure for storing address information
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <fcntl.h>

#define TRUE 1
#define MAX_CMD_SIZE 1024

void copyFile(char *src, char *dst)
{
    char c;
    FILE *srcFile = fopen(src, "rb");
    if (srcFile == NULL)
    {
        printf("Cannot open file %s\n", src);
        return;
    }
    FILE *dstFile = fopen(dst, "wb");
    if (dstFile == NULL)
    {
        printf("Cannot open file %s\n", dst);
        return;
    }
    while (fread(&c, 1, 1, srcFile) == 1)
    {
        fwrite(&c, 1, 1, dstFile);
    }
    fclose(srcFile);
    fclose(dstFile);
}

void myPipe(char cmd[MAX_CMD_SIZE])
{
    char *tok1;
    char *tok2; // 1 = left process, 2 = right
    tok2 = strtok(cmd, "|");
    tok1 = tok2;
    tok2 = strtok(NULL, "|");

    int fd[2];
    if (pipe(fd) == -1)
    {
        return 2;
    }
    int pid1 = fork();
    if (pid1 < 0)
    {
        return 2;
    }
    if (pid1 == 0)
    {
        // we stand on the left process child
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        char *token;
        token = strtok(tok1, " ");
        char *param[1024];
        int i = 0;
        while (token != NULL)
        {
            param[i++] = token;
            token = strtok(NULL, " ");
        }
        param[i] = NULL;
        printf("%s", param[0]);
        if (execvp(param[0], param) == -1)
        {
            printf("error1");
            return 1;
        }
    }

    int pid2 = fork();
    if (pid2 < 0)
    {
        return 2;
    }
    if (pid2 == 0)
    {
        // we stand on the right process child
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        char *token;
        token = strtok(tok2, " ");
        char *param[1024];
        int i = 0;
        while (token != NULL)
        {
            param[i++] = token;
            token = strtok(NULL, " ");
        }
        param[i] = NULL;
        if (execvp(param[0], param) == -1)
        {
            printf("error");
            return 1;
        }
    }
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    close(fd[0]);
    close(fd[1]);
}

int main()
{
    char cmd[MAX_CMD_SIZE] = ""; // BUFFER
    while (TRUE)
    {
        fgets(cmd, MAX_CMD_SIZE, stdin);
        cmd[strlen(cmd) - 1] = '\0';

        if (strchr(cmd, '|') != NULL)
        {
            myPipe(cmd);
        }
        else if ((strchr(cmd, '>') != NULL) || (strchr(cmd, '<') != NULL))
        {
            char *tok1; // process
            char *tok2; // file
            if ((strchr(cmd, '<') != NULL))
            {
                tok2 = strtok(cmd, "<");
                tok1 = tok2;
                tok2 = strtok(NULL, "<");
            }
            else if ((strchr(cmd, '>') != NULL))
            {
                tok1 = strtok(cmd, ">");
                tok2 = tok1;
                tok1 = strtok(NULL, ">");
            }
            int pid = fork();
            if (pid < 0)
            {
                return 2;
            }
            if (pid == 0)
            {
                if (tok2[0] == ' ')
                    tok2++;
                int file = open(tok2, O_WRONLY | O_CREAT, 0777);
                if (file == -1)
                {
                    printf("error open file '<'");
                    return 2;
                }
                dup2(file, STDOUT_FILENO);
                close(file);

                char *token;
                token = strtok(tok1, " ");
                char *param[1024];
                int i = 0;
                while (token != NULL)
                {
                    param[i++] = token;
                    token = strtok(NULL, " ");
                }
                param[i] = NULL;
                printf("%s", param[0]);
                if (execvp(param[0], param) == -1)
                {
                    printf("error1");
                    return 1;
                }
            }
        }
        else if ((strchr(cmd, '}') != NULL) || (strchr(cmd, '{') != NULL))
        {
            char *tok = cmd;
            if ((strchr(cmd, '{') != NULL))
            { // server '{ PORT'
                int pid = fork();
                if (pid < 0)
                {
                    return 2;
                }
                tok += 2; // to get port number
                if (pid == 0)
                {
                    if (execlp("nc", "nc", "-l", tok, NULL) == -1)
                    {
                        printf("error1");
                        return 1;
                    }
                }
            }
            else if ((strchr(cmd, '}') != NULL))
            {
                char *tok1;
                char *tok2;
                int pid = fork();
                if (pid < 0)
                {
                    return 2;
                }
                if (cmd[0] == '}')
                {
                    tok += 2;
                    tok2 = strtok(tok, ":");
                    tok1 = tok2;
                    tok2 = strtok(NULL, ":");
                    if (pid == 0)
                    {
                        if (execlp("nc", "nc", tok1, tok2, NULL) == -1)
                        {
                            printf("error1");
                            return 1;
                        }
                    }
                }
                else
                {
                    tok2 = strtok(cmd, "}");
                    tok1 = tok2;
                    tok2 = strtok(NULL, "}");
                    char *ip;
                    char *port;
                    port = strtok(tok2, ":");
                    ip = port;
                    port = strtok(NULL, ":");
                    printf("ip: %s", ip);
                    printf("port: %s", port);

                    if (pid == 0)
                    {
                        char input[MAX_CMD_SIZE];
                        strcpy(input, tok1);
                        strcat(input, "| nc");
                        strcat(input, ip);
                        strcat(input, " ");
                        strcat(input, port);
                        printf("input: %s", input);
                        myPipe(input);
                    }
                }
            }
        }

        else if (!strncmp(cmd, "DIR", 3))
        {
            DIR *d;
            struct dirent *dir;
            d = opendir(".");
            if (d)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                    {
                        continue;
                    }
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            }
        }
        else if (!strncmp(cmd, "COPY", 4))
        {
            // EXTRACT THE SRC AND DEST NAMES
            char src[MAX_CMD_SIZE];
            char dst[MAX_CMD_SIZE];
            int index = 5;
            int destIndex = 0;
            while (cmd[index] != ' ' && cmd[index] != '\0')
            {
                src[index - 5] = cmd[index];
                index++;
            }

            src[index - 5] = '\0';
            index++;
            for (int i = index; i < strlen(cmd); ++i)
            {
                dst[destIndex] = cmd[i];
                destIndex++;
            }

            dst[destIndex] = '\0';
            // COPYING FILE
            copyFile(src, dst);
        }

        int pid = fork();
        if (pid < 0)
        {
            return 2;
        }
        if (pid == 0)
        {
            char *token;
            token = strtok(cmd, " ");
            char *param[1024];
            int i = 0;
            while (token != NULL)
            {
                param[i++] = token;
                token = strtok(NULL, " ");
            }
            param[i] = NULL;

            if (execvp(param[0], param) == -1)
            {
                printf("error");
                return 1;
            }
        }
    }
}