#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define LOOP_COUNT 1000000

int main(int argc, char *argv[])
{
    struct timespec start, end;
    int fd, i;
    struct flock lock;

    // Open the file
    fd = open("test.txt", O_RDWR | O_CREAT);
    if (fd == -1)
    {
        perror("Failed to open file");
        return 1;
    }

    // Set up the lock structure
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    // Get start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Repeat the lock/unlock loop LOOP_COUNT times
    for (i = 0; i < LOOP_COUNT; i++)
    {
        // Lock the file
        if (fcntl(fd, F_SETLKW, &lock) == -1)
        {
            perror("Failed to lock file");
            return 1;
        }

        // Do something with the locked file here

        // Unlock the file
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) == -1)
        {
            perror("Failed to unlock file");
            return 1;
        }
    }
    // Get end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    double time_secs = (double)elapsed_time / 1000000000.0;
    printf("wake a task using fcntl -%f sec\n", time_secs);

    // Close the file
    close(fd);
    return 0;
}
