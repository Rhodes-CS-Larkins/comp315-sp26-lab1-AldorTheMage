/*
 * pong.c - UDP ping/pong server code
 *          author: Aidan Scoren
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch;
  int nping = 1;                    // default packet count
  char *pongport = strdup(PORTNO);  // default port

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    default:
      fprintf(stderr, "usage: pong [-n #pings] [-p port]\n");
    }
  }

  // pong implementation goes here.
  printf("nping: %d pongport: %s\n", nping, pongport);
  
  //get addr info call and initialization
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int sockfd = -1;
  
  memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;

    if((rv = getaddrinfo(NULL, pongport, &hints, &servinfo)) !=0){
      perror("server adder info error");
    return -1;
    }

    p = servinfo;
     //socket call 

    if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
      perror("server socket error");
      return -1;
    }


    //bind call
    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      perror("bind error");
      return -1;
    }

    //main loop
    for(int i = 0; i<nping; i++){

    char buffer[1024];
      
    struct sockaddr_in client_addr; // saves client info
    socklen_t addr_len = sizeof(client_addr);
    
    if((rv =recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &addr_len)) <0){
    perror("receiving error");
    return -1;
    }
    printf("\npong[%d]: received packet from", nping);
    int arraysize = rv/sizeof(char);

    for(int j=0;j<arraysize;j++){
    buffer[j]++;
    }
    
    if((sendto(sockfd, buffer, sizeof(buffer),0,p->ai_addr,p->ai_addrlen)) == -1){
    perror("sending error");
    return -1;
    }

    }
  freeaddrinfo(servinfo);
  close(sockfd);
  printf("\nexiting");
  return 0;
}

