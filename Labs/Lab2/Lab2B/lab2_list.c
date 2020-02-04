#include "SortedList.h"
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
#include <signal.h>
#include <time.h>

int opt_threads;
int opt_iterations;
int nThreads = 1;
int nIterations = 1;
int nLists;
int opt_yield;
int opt_sync;

char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456789";

//Locks
pthread_mutex_t *mutex;
int *spin;

SortedList_t **list;
SortedListElement_t **element;
long *timer;

void sigHandler(int arg) {
  if (arg == SIGSEGV) {
    fprintf(stderr, "ERROR: segmentation fault\n");
    exit(2);
  }
}
/*
int hashkey(const char *arg) {
  return (arg[0] % nLists);
  }*/

void* thread_worker(void* threadNum) {
  int x = *((int*)threadNum);
  struct timespec start, finish;
  int i;
  for (i = 0; i < nIterations; i++){
    int list_num = ((int) *element[x][i].key) % nLists;

    if (opt_sync == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&mutex[list_num]);
      clock_gettime(CLOCK_MONOTONIC, &finish);
      SortedList_insert(list[list_num], &element[x][i]);
      pthread_mutex_unlock(&mutex[list_num]);
      timer[x] += (1000000000L * (finish.tv_sec - start.tv_sec)) + finish.tv_nsec - start.tv_nsec;
    }
    else if(opt_sync == 's') {
      while(__sync_lock_test_and_set(&spin[list_num], 1));
      SortedList_insert(list[list_num], &element[x][i]);
      __sync_lock_release(&spin[list_num]);
    }
    else
      SortedList_insert(list[list_num], &element[x][i]);
  }

  int Length = 0;
  if (opt_sync == 'm')
    for (i = 0; i < nLists; i++) {
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&mutex[i]);
      clock_gettime(CLOCK_MONOTONIC, &finish);
      timer[x] += (1000000000L * (finish.tv_sec - start.tv_sec)) + finish.tv_nsec - start.tv_nsec;
      Length += SortedList_length(list[i]);
      pthread_mutex_unlock(&mutex[i]);
    }

  else if(opt_sync == 's') 
    for (i = 0; i < nLists; i++){
      while(__sync_lock_test_and_set(&spin[i], 1));
      Length += SortedList_length(list[i]);
      __sync_lock_release(&spin[i]);
    }
  else
    for (i = 0; i < nLists; i++)
      Length += SortedList_length(list[i]);


  SortedListElement_t *current;
  for (i = 0; i < nIterations; i++){
    int list_num = ((int)*element[x][i].key) % nLists;

    if (opt_sync == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&mutex[list_num]);
      clock_gettime(CLOCK_MONOTONIC, &finish);
      current = SortedList_lookup(list[list_num], element[x][i].key);
      SortedList_delete(current);
      pthread_mutex_unlock(&mutex[list_num]);
      timer[x] += (1000000000L * (finish.tv_sec - start.tv_sec)) + finish.tv_nsec - start.tv_nsec;
    }
    else if(opt_sync == 's') {
      while(__sync_lock_test_and_set(&spin[list_num], 1));
      current = SortedList_lookup(list[list_num], element[x][i].key);
      SortedList_delete(current);
      __sync_lock_release(&spin[list_num]);
    }
    else {
      current = SortedList_lookup(list[list_num], element[x][i].key);
      SortedList_delete(current);
    }
  }

  return NULL;
}


int main(int argc, char *argv[]) {
  int c;
  

  static struct option long_options[] =
    {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {"lists", required_argument, 0, 'l'},
      {0,0,0,0}
    };

  int i;

  while (1) {
    c = getopt_long(argc, argv, "", long_options, NULL);
    if (c == -1) break;

    switch (c)
      {
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
	for (i = 0; i < (int)strlen(optarg); i++) {
	  if (optarg[i] == 'i')
	    opt_yield |= INSERT_YIELD;
	  if (optarg[i] == 'd')
	    opt_yield |= DELETE_YIELD;
	  if (optarg[i] == 'l')
	    opt_yield |= LOOKUP_YIELD;
	}
	break;

      case 's':
	opt_sync = optarg[0];
	if (opt_sync == 'm') {
	  //pthread_mutex_init(&mutex, NULL);
	  break;
	}
	if (opt_sync == 's') {
	  
	  break;
	}
	    
      case 'l':
	nLists = atoi(optarg);
	break;
	    
      default:
	fprintf(stderr, "Bad argument! Terminating program.\n");
	exit(1);
      }
  }

  element = (SortedListElement_t**) malloc(nThreads * sizeof(SortedListElement_t*));
  if (element == NULL) {
    fprintf(stderr, "Could not allocate memory for elements.\n");
    exit(1);
  }

  //Generate random keys
  srand(time(NULL));
  element = (SortedListElement_t**) malloc(nThreads*sizeof(SortedListElement_t*));
  int j;
  for (i = 0; i < nThreads; i++) {
    element[i] = (SortedListElement_t*)malloc(nIterations * sizeof(SortedListElement_t));
    for (j = 0; j < nIterations; j++) {
      //max length of key is 5
      char* key = malloc((6) * sizeof(char));
      int k;
      for (k = 0; k < 5; k++)
	key[k] = (char)(rand() % 13);
      key[5] = '\0';
      element[i][j].key = key;
    }
  }

  if (nThreads <= 0)
    nThreads = 1;
  if (nIterations <= 0)
    nIterations = 1;  
  
  if (opt_sync == 'm') {
    for (i = 0; i < nLists; i++) {
      //allocate memory and init
      mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * nLists);
      pthread_mutex_init(&mutex[i], NULL);
    }
  }

  else if (opt_sync == 's') {
    //allocate memory and init
    spin = (int*)malloc(sizeof(int) * nLists);
    for (i=0; i<nLists; i++)
      spin[i] = 0;
  }

    
  list = (SortedList_t**)malloc(nLists * sizeof(SortedList_t*));
  if (list == NULL) {
    fprintf(stderr, "Could not allocate memory for keys\n");
    exit(1);
  }

  //setting up linked list
  for (i=0; i<nLists; i++) {
    list[i] = (SortedList_t*) malloc(sizeof(SortedList_t));
    list[i]->prev = list[i];
    list[i]->next = list[i];
    list[i]->key = NULL;
  }

  //printf("About to execute seg fault handler");
  signal(SIGSEGV, sigHandler);

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  timer = (long*)malloc(nThreads * sizeof(long));
  pthread_t *threadID = malloc(nThreads * sizeof(pthread_t));
  int *index = (int*) malloc(nThreads*sizeof(pthread_t));

  if (threadID == NULL) {
    fprintf(stderr, "ERROR allocating memory for threads\n");
    exit(1);
  }
  //printf("Reached start time\n");
  struct timespec start, finish;

  //start clock;
  clock_gettime(CLOCK_MONOTONIC, &start);
  //printf("about to create threads\n");
  
  //create threads
  for (i=0; i< nThreads; i++) {
    index[i] = i;
    pthread_create(&threadID[i], NULL, thread_worker, &index[i]);
    //fprintf(stdout, "Created threadID[%ld]\n", i);
  }

  //join (wait) threads
  for (i = 0; i < nThreads; i++) {
    pthread_join(threadID[i], NULL);
    //printf("joined threadID[%ld]\n", i);
  }
  
  //finish clock
  clock_gettime(CLOCK_MONOTONIC, &finish);
  long y = nIterations * nThreads * 2;
  long run_time = 1000000000L * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
  int mutex_run_time = 0;
  if (opt_sync == 'm') {
    //increment mutex time for all threads
    for (i=0; i< nThreads; i++) {
      mutex_run_time += timer[i];
    }
    mutex_run_time = mutex_run_time/y;
  }
  long nOperations = nThreads * nIterations * 3;
  long avgOperations = run_time/nOperations;
  
  switch(opt_yield) {
  case 0:
    fprintf(stdout, "list-none");
    break;
  case 1:
    fprintf(stdout, "list-i");
    break;
  case 2:
    fprintf(stdout, "list-d");
    break;
  case 3:
    fprintf(stdout, "list-id");
    break;
  case 4:
    fprintf(stdout, "list-l");
    break;
  case 5:
    fprintf(stdout, "list-il");
    break;
  case 6:
    fprintf(stdout, "list-dl");
    break;
  case 7:
    fprintf(stdout, "list-idl");
    break;
  default:
    break;
  }
    
  switch(opt_sync) {
  case 0:
    fprintf(stdout, "-none");
    break;
  case 's':
    fprintf(stdout, "-s");
    break;
  case 'm':
    fprintf(stdout, "-m");
    break;
  default:
    break;
  }
    
  //long num_lists = 1;

  //printing
  printf(",%d,%d,%d,%ld,%ld,%ld,%d\n",nThreads, nIterations, nLists, nOperations, run_time, avgOperations, mutex_run_time);


  free(element);
  free(threadID);

  if (opt_sync == 'm') {
    for (i=0; i< nLists; i++)
      pthread_mutex_destroy(&mutex[i]);
  }

  exit(0);
}






