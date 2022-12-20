#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREAD_COUNT 5
pthread_mutex_t mutex;
pthread_cond_t cond;
int count = 0;


void *increment_count(void *arg)
{
    for (size_t i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        count++;
        for (size_t i = 0; i < 500; i++)
        {
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[THREAD_COUNT];
    struct timespec start, end;

    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create the threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, increment_count, NULL);
    }

    // Wait for the threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Wait until count becomes greater than 0
    pthread_mutex_lock(&mutex);
    while (count == 0)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    // Stop timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("wake a task using lock -%f sec\n", elapsed);


    // Clean up
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
