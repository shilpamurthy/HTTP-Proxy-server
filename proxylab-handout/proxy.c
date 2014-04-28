#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"
#include "cache.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including these long lines in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *accept_hdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding_hdr = "Accept-Encoding: gzip, deflate\r\n";

//pthread_rwlock_t lock;
//pthread_rwlock_t host_lock;

//struct 

void doit(int fd);
int safe_scan(int n);
int safe_rio(int n);
void *executeT (void *p);

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  int listenfd, clientlen, port;
  int *connfd;
  struct sockaddr_in clientaddr;
  pthread_t thread;


  //Get the port number and listen to requests on thatport.
  port = atoi(argv[1]);

  listenfd = Open_listenfd(port);


  while (1)
  {
    clientlen = sizeof(clientaddr);
    connfd = malloc(sizeof(int));
    //accept the connection and open a new descriptor
    *connfd = Accept(listenfd,(SA *)&clientaddr,(unsigned int *)&clientlen);

    Pthread_create(&thread, NULL, executeT, connfd);
  }
    

    return 0;
}

void *executeT (void* p){

  int fd = *((int*)p);
  free(p);

  Pthread_detach(pthread_self());

  doit(fd);
  close(fd);

  return NULL;
}

void doit(int fd)
{
  int isnot_safe;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], 
  temp[MAXLINE], requestbuf[MAXLINE], host[MAXLINE], headers[MAXLINE],
  path[MAXLINE];
  char *data = malloc(MAX_OBJECT_SIZE*sizeof(char));
  size_t dataLen = 0;
  char *dataBuffer = malloc(MAX_OBJECT_SIZE*sizeof(char));
  char* port_pointer;
  int port = 80, len;
  rio_t rio, rio1;
  int serverConn_fd;

  //associate the file desrpitor
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  
  //parse the buffer and into the method, uri and version
  isnot_safe = safe_scan(sscanf(buf, "%s %s %s\r\n", method, uri, version));

  if (isnot_safe)
    return;

    if (strcasecmp(method, "GET")) {
    printf("501 Not Implemented Proxy does not handle this method");
    return;
    }
    //printf("Does it reach here after Get?\n");

    //parse the uri properly
    if (strstr(uri, "https://") || strstr(uri, "http://"))
    {
        isnot_safe = safe_scan(sscanf(uri, /*"%[^'/']%s"*/ "http://%s",uri));

        if (isnot_safe)
          return;

        //get rid of the two slashes (//) after parsing.
        //memmove(uri, uri+2, strlen(uri));
    }

    printf("%s\n", uri);
    if(strchr(uri, '/') != NULL) {
    isnot_safe = safe_scan(sscanf(uri, "%[^'/']%s", host, path));

        if (isnot_safe)
          return;
    }
    else {
    strcpy(path, "/");
    strcpy(host, uri);
    }
    //printf("HOST %s\n", host);
    if ((port_pointer = strchr(host, ':')) != NULL){
    isnot_safe = safe_scan(sscanf(port_pointer, ":%d", &port));

        if (isnot_safe)
          return;

    isnot_safe = safe_scan(sscanf(host, "%[^0-9:]", host));

        if (isnot_safe)
          return;
    }

    sprintf(requestbuf, "GET %s HTTP/1.0\r\nHost: %s\r\n", path, host);

    serverConn_fd = Open_clientfd(host, port);
    Rio_readinitb(&rio1, serverConn_fd);

    isnot_safe = safe_rio(rio_writen(serverConn_fd, requestbuf, 
                strlen(requestbuf)));

    if (isnot_safe)
        return;

    strcpy(requestbuf, user_agent_hdr);
    isnot_safe = safe_rio(rio_writen(serverConn_fd, requestbuf, strlen(requestbuf)));

    if (isnot_safe)
        return;

    strcpy(requestbuf, accept_hdr);
  isnot_safe = safe_rio(rio_writen(serverConn_fd, requestbuf, strlen(requestbuf)));

    if (isnot_safe)
        return;

    strcpy(requestbuf, accept_encoding_hdr);
    isnot_safe = safe_rio(rio_writen(serverConn_fd, requestbuf, strlen(requestbuf)));

    if (isnot_safe)
        return;

    isnot_safe = safe_rio(rio_writen(serverConn_fd,
      "Proxy-Connection: close\r\nConnection:\tclose\r\n\r\n", 52));

    if (isnot_safe)
        return;


    //Get requests by clients that are not already passed and pass them through
    while((len = rio_readlineb(&rio, requestbuf, MAXBUF)) && 
          strcmp(requestbuf, "\r\n"))
    {
      isnot_safe = safe_scan(sscanf(requestbuf, "%s ",headers));

        if (isnot_safe)
          return;

      if(!strstr(headers, "Host:") && !strstr(headers, "Connection:") &&
         !strstr(headers, "User-Agent:") && !strstr(headers,"Accept-Encoding:")
         && !strstr(headers,"Accept:") && !strstr(headers,"Proxy-Connection:"))

      {

          isnot_safe = safe_rio(rio_writen(serverConn_fd, requestbuf, len));
      
          if (isnot_safe)
            return;     
      }
    }


  while((len = rio_readnb(&rio1, data, len)) != 0){
    dataLen += len;

    if (MAX_OBJECT_SIZE > dataLen){
      memcpy(dataBuffer,data,len);
      dataBuffer += len;
    }
    rio_writen(fd, data, len);
  }

  free(data);

  return;

}

int safe_scan(int n)
{
  if (n == EOF)
  {
    printf("Error in scanning input in %s, line %d\n", __FILE__, __LINE__);
    return 1;
  }

  return 0;
}

int safe_rio(int n)
{
  if (n < 0)
  {
    printf("Error in RIO usage, in %s. line %d\n", __FILE__, __LINE__);
    return 1;
  }

  return 0;
}
