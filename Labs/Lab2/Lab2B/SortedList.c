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
#include <sched.h>
#include <time.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  if (list==NULL || element==NULL)
    return;

  if (opt_yield & INSERT_YIELD)
    sched_yield();
  
  SortedList_t *current = list; 
  
  //if list initially empty
  if (current->next == current && current->prev == current) {
    element->prev = current;
    current->next = list;
  }

  //if list initially NOT empty
  else {     
    //finding point of entry
    SortedList_t *current = list;
    while (current->next != list && strcmp(current->next->key, element->key) <=0) {
      current = current->next;
    }
    
    element->next = current->next;
    element->next->prev = element;
    element->prev = current;
    current->next = element;
    
    
    /*
    //middle
    if (current->next != list && current->prev != list) {
      current->next->prev = element;
      element->prev = current;
      element->next = current->next;
      current->next = element;
    }
    
    //tail
    else if (current->next == list && current->prev != list) {
      element->next = list;
      current->next = element;
      element->prev = current;
      list->prev = element;
    }
    
    //head
    else if (current->next != list && current->prev == list) {
      list->next = element;
      element->next = current;
      element->prev = list;
      current->prev = element;
    }*/
  }
}


int SortedList_delete(SortedListElement_t *element) {
  if (!element || element->next->prev != element || element->prev->next != element)
    return 1;
  if (opt_yield & DELETE_YIELD)
    sched_yield();

  //head
  if (element->prev == NULL) {
    element->next->prev = NULL;
    element->next = NULL;
    element->prev = NULL;
  }

  //tail
  else if (element->next == NULL) {
    element->prev->next = NULL;
    element->next = NULL;
    element->prev = NULL;
  }

  else {
    element->prev->next = element->next;
    element->next->prev = element->prev;
    element->next = NULL;
    element->prev = NULL;
  }

  return 0;
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  if (!list || !key)
    return NULL;

  SortedListElement_t *current = list;

  while (current->next != list) {

    if (strcmp(key, current->key) == 0)
      return current;

    if (opt_yield & LOOKUP_YIELD)
      sched_yield();

    current = current->next;
  }

  return NULL;
}


int SortedList_length(SortedList_t *list) {
  if (!list)
    return -1;

  SortedListElement_t *current = list;

  if (current->next->prev != current || current->prev->next != current)
    return -1;

  int count = 0;
  while (current->next != list) {
    
    if (current->next->prev != current || current->prev->next != current)
      return -1;

    count++;
    if (opt_yield & LOOKUP_YIELD)
      sched_yield();
    
    current = current->next;
  }

  return count;
}
