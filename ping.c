/*
 * ping.c - UDP ping/pong client code
 *          author: Aidan Scoren
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch, errors = 0;
  int nping = 1;                        // default packet count
  char *ponghost = "localhost"; // default host
  char *pongport = PORTNO;      // default port
  int arraysize = 100;                  // default packet size

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'h':
      ponghost = strdup(optarg);
      break;
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    case 's':
      arraysize = atoi(optarg);
      break;
    default:
      fprintf(stderr, "usage: ping [-h host] [-n #pings] [-p port] [-s size]\n");
    }
  }

  // UDP ping implemenation goes here
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int sockfd = -1;

  //intititializing the array/hints
  char arr[arraysize];
  memset(arr,200,arraysize);


  memset(&hints, 0, sizeof hints);
    //this is specifing IPV4, UDP as the protocol, and the non-stream socket type
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;


    //basic addr info check, if rv !=0 it's an error
    if((rv = getaddrinfo(ponghost, pongport, &hints, &servinfo)) !=0){
      fprintf(stderr,"getaddrinfo: %s/n", gai_strerror(rv));

      return 1;
      }
    p = servinfo;

    if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
        perror("Client Socket Error");
        freeaddrinfo(servinfo);
        return -1;
        }
  
    /* This is the section where we loop and send the
     * packets out to the server socket. 
     */

  double totalTime = 0;
  double time1 = 0;
  double roundTripTime = 0;
      int error = 0;

  for(int i =0; i<nping; i++){
    //timer using the one in util.h
    time1 = get_wctime();

    //sending takes socket file decriptor and addr and sends array
    if((sendto(sockfd,arr,arraysize,0,p->ai_addr,p->ai_addrlen)) == -1){
      perror("sending error");
      return -1;
      }

  //get address of the server
  char buffer[arraysize];
  struct sockaddr_in serv_address; 
  socklen_t addr_len = sizeof(serv_address);

  if(recvfrom(sockfd,buffer,sizeof(buffer), 0,(struct sockaddr *) &serv_address, &addr_len) < 0){
    perror("receving error");
    break;
  }
  
      roundTripTime = get_wctime() - time1;
      for(int j =0;j<arraysize;j++){
        if((unsigned char)buffer[j] != 201){
          error = 1;
        }
      }




  printf("\nping[%d] : round-trip time: %f ms", i, roundTripTime);
  totalTime += roundTripTime;
  }

  if(error == 1){
    printf("\nThere was an error in the modification of the array");
  }
  else{
    printf("\nno errors detected");
  }
  double avg = totalTime/nping;
  printf("\ntime to send %d packets of %d bytes %f ms (%f avg per packet)", nping, arraysize, totalTime, avg); 



  printf("\nnping: %d arraysize: %d errors: %d ponghost: %s pongport: %s", nping, arraysize, errors, ponghost, pongport); 

freeaddrinfo(servinfo);
close(sockfd);
  return 0;
}
