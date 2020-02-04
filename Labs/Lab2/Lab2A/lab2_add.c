#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int opt_threads;
int opt_iterations;
long nThreads;
long nIterations;
int opt_yield;
int opt_sync;
long long counter;

//Locks
pthread_mutex_t mutex;
int spin;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

void addM(long long *pointer, long long value) {
  pthread_mutex_lock(&mutex);
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
  pthread_mutex_unlock(&mutex);
}

void addS(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&spin, 1));
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
  __sync_lock_release(&spin);
}

void addC(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&spin, 1));
  long long sum = *pointer + value;
  
  if (opt_yield)
    sched_yield();

  *pointer = sum;
  __sync_lock_release(&spin);
}

void *thread_worker(void * arg) {
  long i;
  
  if (opt_sync == 'm') {
    for (i=0; i<nIterations;i++) {
      addM(&counter, 1);
      addM(&counter, -1);
    }
  }

  else if (opt_sync == 's') {
    for (i=0; i<nIterations; i++) {
      addS(&counter, 1);
      addS(&counter, -1);
    }
  }

  else if (opt_sync == 'c') {
    for (i=0;i<nIterations;i++) {
      addC(&counter, 1);
      addC(&counter, -1);
    }
  }
  else {
    for (i=0; i<nIterations; i++) {
      add(&counter, 1);
      add(&counter, -1);
    }
  }
  return arg;
}

int main(int argc, char *argv[]) {
  int c;

  static struct option long_options[] = 
    {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", no_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {0,0,0,0}
    };
  
  while (1) {
    c = getopt_long(argc, argv, "", long_options, NULL);
    if (c==-1) break;

    switch(c) {
    case 't':
      opt_threads = 1;
      nThreads = atoi(optarg);
      break;

    case 'i':
      opt_iterations = 1;
      nIterations = atoi(optarg);
      break;

    case 'y':
      opt_yield = 1;
      break;

    case 's':
      opt_sync = optarg[0];
      if (opt_sync == 'm') {
	pthread_mutex_init(&mutex, NULL);
	break;
      }
      else {
	break;
      }
      

    default:
      fprintf(stderr, "Bad argument!\n");
      exit(1);
    }
  }

  counter = 0;
  struct timespec start, finish;
  int i;
  pthread_t *threadID = malloc(nThreads * sizeof(pthread_t));

  //start clock
  clock_gettime(CLOCK_MONOTONIC, &start);

  //create
  for (i=0; i < nThreads; i++) {
    pthread_create(&threadID[i], NULL, &thread_worker, NULL);
  }

  //wait (join)
  for (i=0; i < nThreads; i++) {
    if (pthread_join(threadID[i], NULL) != 0)
      fprintf(stdout,"ERROR on pthread_join\n");
    
  }
  
  //finish clock
  clock_gettime(CLOCK_MONOTONIC, &finish);


  long nOperations = nThreads * nIterations * 2;
  //long avgOperations = runTime / nOperations;
  long run_time = 1000000000L * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;

  //Print
  if (opt_yield == 1 && opt_sync == 'm')
    fprintf(stdout, "add-yield-m,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else if (opt_yield == 0 && opt_sync == 'm')
    fprintf(stdout, "add-m,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else if (opt_yield == 1 && opt_sync == 's')
    fprintf(stdout, "add-yield-s,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else if (opt_yield == 0 && opt_sync == 's')
    fprintf(stdout, "add-s,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else if (opt_yield == 1 && opt_sync == 'c')
    fprintf(stdout, "add-yield-c,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else if (opt_yield == 0 && opt_sync == 'c')
    fprintf(stdout, "add-s,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);
  
  else if (opt_yield == 1)
    fprintf(stdout, "add-yield-none,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);

  else 
    fprintf(stdout, "add-none,%ld,%ld,%ld,%ld,%ld,%lld\n", nThreads, nIterations, nOperations, run_time, run_time/nOperations, counter);



  exit(0);
}
