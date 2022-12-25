#include <pthread.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t mutex;  // Mutex variable


int main() {
  struct timespec start, end;
  
  // Get start time
  clock_gettime(CLOCK_MONOTONIC, &start);
  
  // Initialize mutex
  pthread_mutex_init(&mutex, NULL);
 
  int i;
  for (i = 0; i < 1000000; i++) {
    // Lock mutex
    pthread_mutex_lock(&mutex);
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
  }
  
  // Destroy mutex
  pthread_mutex_destroy(&mutex);
  
  // Get end time
  clock_gettime(CLOCK_MONOTONIC, &end);
  
  // Calculate elapsed time
  long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  double time_secs = (double)elapsed_time / 1000000000.0;
  printf("wake a task using mutex -%f sec\n", time_secs);
  
  return 0;
}
