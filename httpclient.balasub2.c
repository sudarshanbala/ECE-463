#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINE 1024

int open_clientfd(char *hostname, int port) 
{ 
  int clientfd; 
  struct hostent *hp; 
  struct sockaddr_in serveraddr; 
 
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    return -1; /* check errno for cause of error */ 
 
  /* Fill in the server's IP address and port */ 
  if ((hp = gethostbyname(hostname)) == NULL) 
    return -2; /* check h_errno for cause of error */ 
  bzero((char *) &serveraddr, sizeof(serveraddr)); 
  serveraddr.sin_family = AF_INET; 
  bcopy((char *)hp->h_addr,  
        (char *)&serveraddr.sin_addr.s_addr, hp->h_length); 
  serveraddr.sin_port = htons(port); 
 
  /* Establish a connection with the server */ 
  if (connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    return -1; 
  return clientfd; 
} 


/* usage: ./echoclient host port */
int main(int argc, char **argv)
{ 
  int clientfd, port; 
  char *host;
  char request[100];

  host = argv[1]; 
  port = atoi(argv[2]); 
 
  clientfd = open_clientfd(host, port);

  if (clientfd < 0){
    printf("Error opening connection \n");
    exit(0);
  }
 
  bzero(request, 100);
  
  sprintf(request, "GET %s HTTP/1.0\r\n\r\n", argv[3]);

  if (send(clientfd, request, strlen(request), 0) < 0)
     return -1; 

  bzero(request, 100);

  int size_recieved = 0;
  int total_size = 0;
  char chunk[5000];

  while(1) 
  {
    bzero(chunk, 1);
    recv(clientfd,chunk,1,0);
    fputs(chunk, stdout);
   
   if((size_recieved = recv(clientfd,chunk,1,0)) == 0)
        {
            break;
        }
        else
        {
            total_size += size_recieved;
            fputs(chunk, stdout);
        }

    } 

  close(clientfd); 
  exit(0); 
} 



