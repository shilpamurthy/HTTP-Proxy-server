#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"


struct cache_node;

typedef struct cache_node{
  char *file;
  char *url;
  int size;
  struct cache_node *next;
  struct cache_node *prev;
} cache_node; 

cache_node *find (char *url);
void addToFront(cache_node *fileObj);
void delete(cache_node *fileObj);
void takeCareOfSize();