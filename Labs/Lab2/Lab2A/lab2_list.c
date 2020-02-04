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
long nThreads;
long nIterations;
int opt_yield;
int opt_sync;

char *junk = "123456789abcdefghijklmnopqrstuvABCDEFGHIJKLMNOPQRSTUVWXYZ";

pthread_mutex_t mutex;
int spin;

SortedList_t *list;
SortedListElement_t **element;

void sigHandler(int arg) {
	if (arg == SIGSEGV) {
		fprintf(stderr, "ERROR: segmentation fault\n");
		exit(2);
	}
}

void *thread_worker(void *arg) {
  int id = *((int*)arg);
  int j;
  for (j = 0; j < nIterations; j++) {
    if (opt_sync == 'm') {
      pthread_mutex_lock(&mutex);
      SortedList_insert(list, &element[id][j]);
      pthread_mutex_unlock(&mutex);
    }
    else if (opt_sync == 's') {
      while(__sync_lock_test_and_set(&spin,1));
      SortedList_insert(list, &element[id][j]);
      __sync_lock_release(&spin);
    }
    else
      SortedList_insert(list, &element[id][j]);
  }
  int Length = 0;
  if (opt_sync == 'm'){
    pthread_mutex_lock(&mutex);
    Length = SortedList_length(list);
    pthread_mutex_unlock(&mutex);
  }
  else if (opt_sync == 's') {
    while(__sync_lock_test_and_set(&spin, 1));
    Length = SortedList_length(list);
    __sync_lock_release(&spin);
  }
  else
    Length = SortedList_length(list);

  SortedListElement_t *current;
  for (j = 0; j < nIterations; j++) {
    if (opt_sync == 'm') {
      pthread_mutex_lock(&mutex);
      current = SortedList_lookup(list, element[id][j].key);
      SortedList_delete(current);
      pthread_mutex_unlock(&mutex);
    }
    else if (opt_sync == 's') {
      while(__sync_lock_test_and_set(&spin, 1 ));
      current  = SortedList_lookup(list, element[id][j].key);
      SortedList_delete(current);
      __sync_lock_release(&spin);
    }
    else {
      current = SortedList_lookup(list, element[id][j].key);
      SortedList_delete(current);
    }    
  }

  Length = SortedList_length(list);
  if (Length > 0) {
    fprintf(stderr, "ERROR list length not zero\n");
    exit(2);
  }

  return NULL;

}

/*
    long x = ((*(int*)arg)*nIterations);
  
  if (opt_sync == 'm')
    pthread_mutex_lock(&mutex);
  if (opt_sync == 's')
		while (__sync_lock_test_and_set(&spin, 1));

	long i;
	for (i = x; i < x + nIterations; i++)
		SortedList_insert(list, element[i]);

	long len = SortedList_length(list);
	SortedListElement_t *toDelete;

	for (i = x; i < x + nIterations; i++) {
		toDelete = SortedList_lookup(list, element[i]->key);

		if (toDelete == NULL) {
			fprintf(stderr, "FAILED to lookup\n");
			exit(2);
		}
		SortedList_delete(toDelete);
	}

	len = SortedList_length(list);
	if (len > 0) {
		fprintf(stderr, "FAILED to delete\n");
		exit(2);
	}

	if (opt_sync == 'm')
		pthread_mutex_unlock(&mutex);
	if (opt_sync == 's')
		__sync_lock_release(&spin);

	return NULL;
*/

	/*
	SortedListElement_t *e = arg;

	if (opt_sync == 'm')
	  pthread_mutex_lock(&mutex);
	if (opt_sync == 's')
	  while(__sync_lock_test_and_set(&spin,1));

	//insert
	long i;
	for (i=0; i<nIterations; i++)
	  SortedList_insert(&list, (SortedListElement_t*)(e+i));

	long len = SortedList_length(&list);
	SortedListElement_t *toDelete;

	char *current = malloc(sizeof(char)*512);
	//lookup & delete
	for (i=0; i<nIterations; i++) {
	  //current = (e+i)->key;
	  strcpy(current, (e+i)->key);
	  toDelete = SortedList_lookup(&list, current);
	  SortedList_delete(toDelete);
	}

	//check to see if all deleted
	if (SortedList_length != 0) {
	  fprintf(stderr, "FAILED deleting list elements\n");
	  exit(2);
	}

	if (opt_sync == 'm')
	  pthread_mutex_unlock(&mutex);
	if (opt_sync == 's')
	  __sync_lock_release(&spin);

	return NULL;

	
}*/

int main(int argc, char *argv[]) {
  int c;
  //  pthread_t *threadID;
  //int *tn;

  static struct option long_options[] =
  {
	{"threads", required_argument, 0, 't'},
	{"iterations", required_argument, 0, 'i'},
	{"yield", required_argument, 0, 'y'},
	{"sync", required_argument, 0, 's'},
	{0,0,0,0}
  };

  long i;

  while (1) {
	  c = getopt_long(argc, argv, "", long_options, NULL);
	  if (c == -1) break;

	  switch (c)
	  {
	  case 't':
		  opt_threads = 1;
		  nThreads = atoi(optarg);
		  //threadID = (pthread_t*) malloc(nThreads *sizeof(pthread_t));
		  //tn = (int*) malloc(nThreads *sizeof(int));
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
			  pthread_mutex_init(&mutex, NULL);
			  break;
		  }
		  if (opt_sync == 's') {
			  spin = 0;
			  break;
		  }

	  default:
		  fprintf(stderr, "Bad argument!\n");
		  exit(1);
	  }
  }
  
  if (nThreads <= 0)
	  nThreads = 1;
  if (nIterations <= 0)
	  nIterations = 1;
  printf("About to execute sighandler\n");
  signal(SIGSEGV, sigHandler);

  /*  long num_list_elements = nThreads * nIterations;

  element = malloc(num_list_elements * sizeof(SortedListElement_t));
  if (element == NULL) {
    fprintf(stderr, "ERROR allocating memory for SortedListElement_t\n");
    exit(1);
  }

  list = (SortedList_t *) malloc(sizeof(SortedList_t));
  list->key = NULL;
  list->prev = list;
  list->next = list;

  for (i=0; i<num_list_elements; i++) {
    element[i] = malloc(sizeof(SortedListElement_t));
    element[i]->next = NULL;
    element[i]->prev = NULL;

    char *newChar = malloc(sizeof(char)*(11));
    int j;
    for (j=0; j<10; j++) {
      newChar[j] = (rand() % 13);
    }
    newChar[10] = '\0';
    element[i]->key = newChar;
    } */ 
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  long num_list_elements = nThreads * nIterations;
  element = malloc(sizeof(SortedListElement_t) * num_list_elements);
  list = malloc(sizeof(SortedList_t));
  list->prev = list;
  list->next = list;
  list->key = NULL;

  if (element == NULL) {
    fprintf(stderr, "Could not allocate memory for elements.\n");
    exit(1);
  }
  
  if (list == NULL) {
    fprintf(stderr, "Could not allocate memory for keys\n");
    exit(1);
  }

  //Generating random keys
  for (i = 0; i < num_list_elements; i++) {
	  element[i] = malloc(sizeof(SortedListElement_t));
	  element[i]->prev = NULL;
	  element[i]->next = NULL;

	  char *keyz = malloc(sizeof(char)*(11));
	  int j;
	  for (j = 0; j < 10; j++) {
		  keyz[j] = junk[rand() % strlen(junk)];
	  }
	  keyz[10] = '\0';
	  element[i]->key = keyz;
	  /*
	  keys[i] = malloc(sizeof(char) * 256);
	  if (keys[i] == NULL) {
		fprintf(stderr, "Could not allocate memory for keys.\n");
		exit(1);
	  }
	  int j;
	  for (j = 0; j < 255; j++) {
		keys[i][j] = rand() % 94 + 33;
	  }
	  keys[i][255] = '\0';
	  (element + i)->key = keys[i]; */
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////

  //Thread creation process
  //int *tn = (int*)malloc(nThreads * sizeof(int));
  pthread_t *threadID = malloc(nThreads * sizeof(pthread_t));
  if (threadID == NULL) {
    fprintf(stderr, "ERROR allocating memory for threads\n");
    exit(1);
  }
  printf("Reached start time\n");
  struct timespec start, finish;

  //start clock;
  clock_gettime(CLOCK_MONOTONIC, &start);
  printf("about to create threads\n");
  //create threads

  int index[nThreads];
  for (i=0; i< nThreads; i++) {
    index[i] = i;
    pthread_create(&threadID[i], NULL, &thread_worker, &index[i]);
    fprintf(stdout, "Created threadID[%ld]", i);
  }

  //join threads
  for (i = 0; i < nThreads; i++) {
    //printf("joined threadID[%ld]\n", i);
    pthread_join(threadID[i], NULL);
    printf("joined threadID[%ld]\n", i);
  }
  
  //finish clock
  clock_gettime(CLOCK_MONOTONIC, &finish);
  
  
  long run_time = 1000000000L * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
  long nOperations = nThreads * nIterations * 3;
  long avgOperations = run_time/nOperations;
  

  if (SortedList_length(list) != 0) {
    fprintf(stderr, "Length of list not 0 at end.\n");
    exit(2);
  }

  fprintf(stdout, "list");

  switch(opt_yield) {
  case 0:
    fprintf(stdout, "-none");
    break;
  case 1:
    fprintf(stdout, "-i");
    break;
  case 2:
    fprintf(stdout, "-d");
    break;
  case 3:
    fprintf(stdout, "-id");
    break;
  case 4:
    fprintf(stdout, "-l");
    break;
  case 5:
    fprintf(stdout, "-il");
    break;
  case 6:
    fprintf(stdout, "-dl");
    break;
  case 7:
    fprintf(stdout, "-idl");
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
    
  
  
  
  long num_lists = 1;

  // print rest of data
  fprintf(stdout, ",%ld,%ld,%ld,%ld,%ld,%ld\n", nThreads, nIterations, num_lists, nOperations, run_time, avgOperations);

  free(element);
  free(threadID);

  if (opt_sync == 'm') {
    pthread_mutex_destroy(&mutex);
  }
  exit(0);
}

