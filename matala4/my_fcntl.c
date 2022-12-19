#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define THREAD_COUNT 2
#define ITERATIONS 1000000

struct thread_arg {
    int fd;
    struct flock fl;
};

void *thread_func(void *arg)
{
    struct thread_arg *targ = (struct thread_arg *)arg;
    int fd = targ->fd;
    struct flock *fl = &targ->fl;
    int lock_result;
    lock_result = fcntl(fd, F_SETLKW, fl);

    if (lock_result == -1) {
        perror("LOCK");
    }

    // Do something with the locked file
    for (int i = 0; i < 1000; i++) {
        // Write the character 'a' to the file
        write(fd, "a", 1);
    }
    
    // Release the lock
    fl->l_type = F_UNLCK;
    fcntl(fd, F_SETLK, fl);
    

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[THREAD_COUNT];
    int fd;
    struct flock fl;
    struct thread_arg targ;
    

    // Open the file for reading and writing
    fd = open("file.txt", O_RDWR | O_CREAT);
    if (fd == -1) {
        // Handle error
    }

    // Initialize the flock structure
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    // Initialize the thread argument structure
    targ.fd = fd;
    targ.fl = fl;

    struct timespec start, end;

    // Use clock_gettime to measure the time it takes to acquire the lock
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    

    // Create the threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, thread_func, &targ);
    }

    // Wait for the threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    // Calculate elapsed time in seconds
    double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;
      printf("wake a task using lock -%f sec\n", elapsed );

    close(fd);

    return 0;
}
