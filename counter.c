#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int nthreads;
pthread_t* threads;
pthread_mutex_t c_lock;
int cont = 1;
unsigned long counter = 0UL;
unsigned long limit = 999999UL;
time_t end;

void* stress(void* arg) {
  int tid = *((int*)arg);
  while(cont) {
    pthread_mutex_lock(&c_lock);
    if (counter == limit) {
      cont = 0;
      end = time(NULL);
      pthread_mutex_unlock(&c_lock);
      break;
    }
    counter += 1;
    printf("Thread %d incrementing counter.\n", tid);
    pthread_mutex_unlock(&c_lock);
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

  time_t start = time(NULL);
  for (int i = 0; i < nthreads; ++i) {
    threadIdxs[i] = i;
    pthread_create(&threads[i], NULL, stress, &threadIdxs[i]);
  }

  for (int i = 0; i < nthreads; ++i) {
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&c_lock);
  free(threads);
  printf("Time taken: %ld seconds\n", end - start);
  printf("\nExiting...\n");

  return 0;
}