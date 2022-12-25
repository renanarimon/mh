#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define LOOP_COUNT 500000

int main(void)
{
    struct timespec start, end;
    int pipefd[2];
    pid_t pid;
    int i;
    char bit = '1';

    // Create the pipe
    if (pipe(pipefd) == -1)
    {
        perror("Failed to create pipe");
        return 1;
    }

    // Fork the process
    pid = fork();
    if (pid == -1)
    {
        perror("Failed to fork process");
        return 1;
    }

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < LOOP_COUNT; i++)
    {
        if (pid == 0)
        {
            // Child process: read from pipe and write to pipe
            if (read(pipefd[0], &bit, 1) != 1)
            {
                perror("Failed to read from pipe");
                return 1;
            }
            if (write(pipefd[1], &bit, 1) != 1)
            {
                perror("Failed to write to pipe");
                return 1;
            }
        }
        else
        {
            // Parent process: write to pipe and read from pipe
            if (write(pipefd[1], &bit, 1) != 1)
            {
                perror("Failed to write to pipe");
                return 1;
            }
            if (read(pipefd[0], &bit, 1) != 1)
            {
                perror("Failed to read from pipe");
                return 1;
            }
        }
    }
    // Stop timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    double time_secs = (double)elapsed_time / 1000000000.0;

    // Wait for child process to finish
    wait(NULL);
    if (pid != 0)
    {
        printf("wake a task using pipe -%f sec\n", time_secs);
    }

    return 0;
}
