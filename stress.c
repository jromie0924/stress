#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int nthreads;
pthread_t* threads;
pthread_mutex_t c_lock;
volatile sig_atomic_t cont = 1;
volatile double* sink;

#define N 1024

void* stress(void* arg) {
  int tid = *((int*)arg);
  double x[N], y[N];
  for (int i = 0; i < N; ++i) {
    x[i] = tid + i * 0.001;
    y[i] = 1.0;
  }
  while(cont) {
    for (int i = 0; i < N; ++i) {
      y[i] = y[i] * 0.999 + x[i];
    }
  }
  sink[tid] = y[0];
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
  sink = malloc(sizeof(double) * nthreads);

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
  free((void*)sink);
  printf("\nExiting...\n");

  return 0;
}