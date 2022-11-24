#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <netinet/in.h> //structure for storing address information
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>

#define TRUE 1
#define MAX_CMD_SIZE 1024


char** splitStr(char* str){
    
    int init_size = strlen(str);
    char* ans[init_size];
	char delim[] = " ";
	char *ptr = strtok(str, delim);

    int i=0;
	while(ptr != NULL)
	{
        ans[i++] = ptr;
		printf("'%s'\n", ptr);
		ptr = strtok(NULL, delim);
	}
	// /* This loop will show that there are zeroes in the str after tokenizing */
	// for (int i = 0; i < init_size; i++)
	// {
	// 	printf("%d ", str[i]); /* Convert the character to integer, in this case
	// 						   the character's ASCII equivalent */
	// }
    return ans;
}


void copyFile(char *src, char *dst) {
    char c;
    FILE *srcFile = fopen(src, "rb");
    if (srcFile == NULL) {
        printf("Cannot open file %s\n", src);
        return;
    }
    FILE *dstFile = fopen(dst, "wb");
    if (dstFile == NULL) {
        printf("Cannot open file %s\n", dst);
        return;
    }
    while (fread(&c, 1, 1, srcFile) == 1) {
        fwrite(&c, 1, 1, dstFile);
    }
    fclose(srcFile);
    fclose(dstFile);
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
            char* tok1; 
            char* tok2; // 1 = left process, 2 = right
            tok2 = strtok(cmd, "|");
            tok1 = tok2;
            tok2 = strtok(NULL, "|");

            int fd[2];
            if(pipe(fd)==-1){
                return 2;
            }
            int pid1=fork();
            if(pid1 < 0){
                return 2;
            }
            if(pid1 == 0 ){
                //we stand on the left process child
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                
                // printf("fn: %s, args: %s", ans[0], args1);
                // execve(fileName1, args1, NULL); ////// TODO
            }

            int pid2=fork();
            if(pid2 < 0){
                return 2;
            }
            if(pid2 == 0 ){
                //we stand on the right process child
                dup2(fd[1], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                char* fileName1;
                char* args1; 
                args1 = strtok(tok1, " ");
                fileName1 = args1;
                args1 = strtok(NULL, " ");
                printf("fn: %s, args: %s", fileName1, args1);
                execve(fileName1, args1, NULL); ////// TODO
            }
            waitpid(pid1, NULL ,0);
            waitpid(pid2, NULL ,0);

            close(fd[0]);
            close(fd[1]);


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
        else if(!strncmp(cmd, "COPY",4)){
            //EXTRACT THE SRC AND DEST NAMES
            char src[MAX_CMD_SIZE];
            char dst[MAX_CMD_SIZE];
            int index = 5;
            int destIndex = 0;
            while (cmd[index] != ' ' && cmd[index] != '\0') {
                src[index-5] = cmd[index];
                index++;
            }

            src[index-5] = '\0';
            index++;
            for (int i = index; i < strlen(cmd); ++i) {
                dst[destIndex] = cmd[i];
                destIndex++;
            }
            
            dst[destIndex] = '\0';
            //COPYING FILE
            copyFile(src, dst);
        }

        
    }
}