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
#include <fcntl.h>
#define N 1000

int record_server(int s, char* filename){
  int ns = 1;
  int nr = 1; 
  int nd = 1;
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd==-1){
    perror("open");
    exit(1);
  }
  FILE *fp_read; FILE *fp_write;
  int flag_read = 0; int flag_write = 0;
  char* data_send = (char*)malloc(N * sizeof(char));
  char* data_recv = (char*)malloc(N * sizeof(char));
 	while(ns > 0 && nd > 0){
     if(flag_write==0){
       if((fp_read = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
       flag_write=1;
     }
    ns = fread(data_send, 1, N * sizeof(char), fp_read);
    ns = send(s, data_send, N * sizeof(char), 0);
    nd = write(fd, data_send, N*sizeof(char));
    while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0) continue;
    if(nr == 0){
      //break;
    }
    if(flag_read==0){
      if((fp_write = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
      flag_read=1;
    }
    nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
    nd = write(fd, data_recv, N*sizeof(char));
  }
	//close(s);
  close(fd);
  pclose(fp_read);
  pclose(fp_write);
	return 0;
}

int record_client(int s, char* filename){
  int ns=1; int nr = 1; int nd=1;
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd==-1){
    perror("open");
    exit(1);
  }
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
      if((fp_write = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
      flag_write = 1;
    }
    nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
    nd = write(fd, data_recv, N*sizeof(char));
    if(flag_read==0){
      if((fp_read = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
      flag_read = 1;
    }
    ns = fread(data_send, 1, N * sizeof(char), fp_read);
    ns = send(s, data_send, N * sizeof(char), 0);
    nd = write(fd, data_send, N*sizeof(char));
  }
  //close(s);
  close(fd);
  pclose(fp_read);
  pclose(fp_write);
	return 0;
}