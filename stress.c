#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int nthreads;
pthread_t* threads;
pthread_mutex_t c_lock;
int cont = 1;

void* stress(void* arg) {
  int tid = *((int*)arg);
  while(cont) {
    // pthread_mutex_lock(&c_lock);
    double a = 5.64345 * tid;
    // printf("Thread ID %d: a = %f\n", tid, a);
    // usleep(500000);
    // pthread_mutex_unlock(&c_lock);
  }
  return 0;
}

void handle_sig(int signum) {
  if (signum == SIGINT) {
    cont = 0;
  }
}

int main() {
  nthreads = 16;
  int threadIdxs[nthreads];
  pthread_mutex_init(&c_lock, NULL);
  threads = malloc(sizeof(pthread_t) * nthreads);

  signal(SIGINT, handle_sig);

  for (int i = 0; i < nthreads; ++i) {
    threadIdxs[i] = i;
    pthread_create(&threads[i], NULL, stress, &threadIdxs[i]);
  }

  for (int i = 0; i < nthreads; ++i) {
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&c_lock);
  free(threads);
  printf("\nExiting...\n");

  return 0;
}