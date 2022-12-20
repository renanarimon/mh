#include <pthread.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t mutex;  // Mutex variable
int shared_resource = 0; // Shared resource

void* thread_func(void* arg) {
  int i;
  for (i = 0; i < 1000000; i++) {
    // Lock mutex
    pthread_mutex_lock(&mutex);
    
    // Modify shared resource
    shared_resource++;
    for (size_t i = 0; i < 500; i++)
    {
    }
    
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
  }
  
  return NULL;
}

int main() {
  struct timespec start, end;
  long elapsed_time;
  
  // Get start time
  clock_gettime(CLOCK_MONOTONIC, &start);
  
  // Initialize mutex
  pthread_mutex_init(&mutex, NULL);
  
  // Create threads
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, thread_func, NULL);
  pthread_create(&thread2, NULL, thread_func, NULL);
  
  // Wait for threads to finish
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  
  // Destroy mutex
  pthread_mutex_destroy(&mutex);
  
  // Get end time
  clock_gettime(CLOCK_MONOTONIC, &end);
  
  // Calculate elapsed time
  elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  
  // printf("Shared resource: %d\n", shared_resource);
  printf("wake a task using lock -%ld sec\n", elapsed_time / 1000000000);
  
  return 0;
}
