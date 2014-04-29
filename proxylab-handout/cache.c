#include "cache.h"

#define MAX_CACHE_SIZE 1049000

int maxSize = 0;
cache_node *start = NULL;
cache_node *end = NULL;
extern pthread_rwlock_t lock;

cache_node *find(char *url)
{
  cache_node *fileObject = start;
  if (fileObject == NULL)
    return NULL;

  pthread_rwlock_rdlock(&lock);
  while (fileObject != NULL)
  {
    if (strcmp(fileObject->url,url) == 0)
    {
      pthread_rwlock_unlock(&lock);
      pthread_rwlock_wrlock(&lock);
      delete(fileObject);
      addToFront(fileObject);
      pthread_rwlock_unlock(&lock);
      return fileObject;
    }

    fileObject = fileObject->next;
  }

  pthread_rwlock_unlock(&lock);
  return NULL;

}

void addToFront (cache_node *fileObj)
{
  if  (start == NULL)
  {
    start = fileObj;
    end = fileObj;
    fileObj->prev = NULL;
    fileObj->next = NULL;
    maxSize+= fileObj->size;
    takeCareOfSize();
    return;
  }

  fileObj->next = start;
  start->prev = fileObj;
  fileObj->prev = NULL;
  start = fileObj;
  maxSize+= fileObj->size;
  takeCareOfSize();
  return;

}

void takeCareOfSize()
{
  cache_node *result;
  while (maxSize > MAX_CACHE_SIZE)
  {
    result = end;
    maxSize -= result->size;
    delete(result);
  }

  return;
}

void delete(cache_node *fileObj)
{
  cache_node *temp, *temp1;
  int size = fileObj->size;
  //Only file object in cache
  if (fileObj->prev==NULL && fileObj->next==NULL)
  {
    start = NULL;
    end = NULL;
    /*free(fileObj->url);
    free(fileObj->file);
    free(fileObj);*/
  }

  //file object is at the beginning of the cache
  if (fileObj->prev==NULL && fileObj->next != NULL)
  {
    start = fileObj->next;
    start->prev = NULL;
   /* free(fileObj->url);
    free(fileObj->file);
    free(fileObj);*/
  }

  //end of cache
  if (fileObj->prev!=NULL && fileObj->next == NULL)
  {
    end = fileObj->prev;
    end->next = NULL;
    /*free(fileObj->url);
    free(fileObj->file);
    free(fileObj);*/

  }

  if (fileObj->prev!=NULL && fileObj->next!=NULL)
  {
    temp = fileObj->prev;
    temp1 = fileObj->next;
    temp->next = temp1;
    temp1->prev = temp;
    /*free(fileObj->url);
    free(fileObj->file);
    free(fileObj);*/
  }

  maxSize -= size;

  return;

}