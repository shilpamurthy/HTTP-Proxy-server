#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"

#define MAX_CACHE_SIZE 1049000


/* We decided to have the cache implemented as a doubly linked list because we initially had an
array and decided that a linked list would save memory and not reduce in speed because we are
only accessing the start and end nodes. In implementing the LRU policy, we only append to the
beginning and delete from the end (modelled more like the explicit list implementation from 
malloc! :)) */
struct cache_node;

typedef struct cache_node{
	char *url;
 	char *file;
  	int size;
  	struct cache_node *next;
  	struct cache_node *prev;
} cache_node; 

void init_cache();
void addToFront(char *file, char *uri, int size);
void delete(cache_node *fileObj);
cache_node *find (char *url);
void takeCareOfSize();
void printCache();
void destroy_file(cache_node *fileObj);