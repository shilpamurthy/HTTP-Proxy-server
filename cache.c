#include "cache.h"

#define MAX_CACHE_SIZE 1049000

int maxSize;
cache_node *start;
cache_node *end;
extern pthread_rwlock_t lock;

/*
 * init initializes the cache.
 */
void init_cache()
{
  start = NULL;
  end = NULL;
  maxSize = 0;
}

/*
 * addToFront adds the irl and file and size to
 * a cachnode and adds it to the front of the list
 */
void addToFront(char *uri, char *file, int size)
{
	cache_node *fileObj = malloc(sizeof(cache_node));
  //The url has to be copied only in this way because
  //the url we are getting corresponds to a pointer
  //in the descriptor that gets closed.
  fileObj->url = malloc(MAXLINE);
  strcpy(fileObj->url,uri);
  //We can assign file this way however because we take
  //care of writing data to the buffer using a temporary
  //variable
  fileObj->file = file;
  fileObj->size = size;

  //There are no elements in the cache
  if  (start == NULL)
  {
    end = fileObj;
    start = end;
    fileObj->prev = NULL;
    fileObj->next = NULL;
    maxSize+= size;
    takeCareOfSize();
    return;
  }

  //There are elements in the cache
  fileObj->next = start;
  start->prev = fileObj;
  fileObj->prev = NULL;
  start = fileObj;
  maxSize+= size;
  takeCareOfSize();
  return;

}

/*
 *Delet, deletes a file object in the cache
 * Thank god for malloc explicit lists :)
 */
void delete(cache_node *fileObj)
{

	//printCache();
  cache_node *temp, *temp1;
  if (fileObj == NULL || start == NULL)
    return;

  int size = fileObj->size;
    maxSize -= size;
  //Case 1; Only file object in cache
  if (fileObj->prev==NULL && fileObj->next==NULL)
  {
    start = NULL;
    end = NULL;
    return;
  }

  //Case 2: file object is at the beginning of the cache
  if (fileObj->prev==NULL && fileObj->next != NULL)
  {
    
    start = fileObj->next;
    start->prev = NULL;
    return;
  }

  //Case 3: end of cache
  if (fileObj->prev!=NULL && fileObj->next == NULL)
  {
    end = fileObj->prev;
    end->next = NULL;
    return;
  }

  //Case 4: middle of cache:
  temp = fileObj->prev;
  temp1 = fileObj->next;
  temp->next = temp1;
  temp1->prev = temp;
  return;

}

/*
 * Find is a function that outputs the file if it is found
 * in the cache and null otherwise
 */
cache_node *find(char *url)
{
  cache_node *fileObject = start;
  if (fileObject == NULL)
    return NULL;

  pthread_rwlock_rdlock(&lock);
  while (fileObject != NULL)
  {
    if ((strcmp(fileObject->url,url)) == 0)
    {
      pthread_rwlock_unlock(&lock);
      pthread_rwlock_wrlock(&lock);
      delete(fileObject);
      addToFront(fileObject->file, fileObject->url, fileObject->size);
      pthread_rwlock_unlock(&lock);
      return fileObject;
    }

    fileObject = fileObject->next;
  }

  pthread_rwlock_unlock(&lock);
  return NULL;

}

/**************************
*****HELPER FUNCTIONS******
***************************/

void takeCareOfSize()
{
  cache_node *result;
  while (maxSize > MAX_CACHE_SIZE)
  {
    result = end;
    delete(result);
    destroy_file(result);
  }

  return;
}

/*
 * We have to free all the pointers
 */
void destroy_file(cache_node *fileObj)
{
  free(fileObj->url);
  free(fileObj->file);
  free(fileObj);
}

/*
 * Helper functionthat prints all the contents of the cache
 */
void printCache()
{
	cache_node *temp = start;

	while(temp != NULL)
	{
		printf("URL : %s, SIZE: %d, SIZEOFCACHE: %d\n", temp->url, temp->size, 
                                                                  maxSize);
	}

}