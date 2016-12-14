#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#define LISTENQ 10
#define MAXLINE 10000

int open_listenfd(int port);
void echo(int connfd);

int main(int argc, char **argv) 
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  port = atoi(argv[1]); /* the server listens on a port passed 
			   on the command line */
  
  listenfd = open_listenfd(port); //SOCKET, BIND, LISTEN 

  while (1) {
    clientlen = sizeof(clientaddr); 
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen); //ACCEPT
    echo(connfd);
    close(connfd);
    }
return 0;
}

int open_listenfd(int port)  
{ 
  int listenfd, optval=1; 
  struct sockaddr_in serveraddr; 
   
  /* Create a socket descriptor */ 
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    return -1; 
  
  /* Eliminates "Address already in use" error from bind. */ 
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  
		 (const void *)&optval , sizeof(int)) < 0) 
    return -1; 
 
  /* Listenfd will be an endpoint for all requests to port 
     on any IP address for this host */ 
  bzero((char *) &serveraddr, sizeof(serveraddr)); 
  serveraddr.sin_family = AF_INET;  
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
  serveraddr.sin_port = htons((unsigned short)port);  
  if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
    return -1; 
 
  /* Make it a listening socket ready to accept 
     connection requests */ 
  if (listen(listenfd, LISTENQ) < 0) 
    return -1; 
 
  return listenfd; 
} 

void echo(int connfd)
{
  char buf[MAXLINE];
  char * path;
  FILE * fd;
  
  read(connfd, buf, MAXLINE);
  path = strtok(buf, " ");
  path = strtok(NULL, " "); 
  path++;
 
  if (access(path, R_OK) == 0)
    {
      write(connfd, "HTTP/1.0 200 OK\r\n\r\n", strlen("HTTP/1.0 200 OK\r\n\r\n"));
      fd = fopen(path, "r");
      while (!feof(fd))
	{
	  memset(buf,0,MAXLINE);
	  fread(buf, sizeof(char), MAXLINE-1, fd);
	  write(connfd, buf, strlen(buf));
	}
      fclose(fd);
    }
  else
    {
      if (errno != EACCES)
	{
	  write(connfd, "HTTP/1.0 404 Not Found\r\n\r\n", strlen("HTTP/1.0 404 Not Found\r\n\r\n"));
	}
      else
	{
	  write(connfd, "HTTP/1.0 403 Forbidden\r\n\r\n", strlen("HTTP/1.0 403 Forbidden\r\n\r\n"));
	}
    } 
  
} 
