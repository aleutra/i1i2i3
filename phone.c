#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#define N 1000

int phone_server(int s){
  int ns=1; int nr = 1;
  FILE *fp_read; FILE *fp_write;
  int flag_read = 0; int flag_write = 0;
  char* data_send = (char*)malloc(N * sizeof(char));
  char* data_recv = (char*)malloc(N * sizeof(char));
 	while(ns > 0){
     if(flag_write==0){
       flag_write=1;
       if((fp_read = popen("rec -t raw -q -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
     }
    ns = fread(data_send, 1, N * sizeof(char), fp_read);
    ns = send(s, data_send, N * sizeof(char), 0);
    while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0) continue;
    if(nr == 0){
      //break;
    }
    if(flag_read==0){
      if((fp_write = popen("play -t raw -q -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
      flag_read=1;
    }
    nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
  }
	//close(s);
  pclose(fp_read);
  pclose(fp_write);
	return 0;
}

int phone_client(int s){
  int ns=1; int nr = 1;
  FILE *fp_read; FILE *fp_write;
  int flag_read = 0; int flag_write = 0;
  char* data_send = (char*)malloc(N * sizeof(char));
  char* data_recv = (char*)malloc(N * sizeof(char));
 	while(ns > 0){
    while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0) continue;
    if(nr == 0){
      //break;
    }
    if(flag_write==0){
      if((fp_write = popen("play -t raw -q -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
      flag_write = 1;
    }
    nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
    if(flag_read==0){
      if((fp_read = popen("rec -t raw -q -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
      flag_read = 1;
    }
    ns = fread(data_send, 1, N * sizeof(char), fp_read);
    ns = send(s, data_send, N * sizeof(char), 0);
  }
  //close(s);
  pclose(fp_read);
  pclose(fp_write);
	return 0;
}