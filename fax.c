#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int fax_send(int s, char* filename){
	int n=1; int m=1;
  int fd = open(filename, O_RDONLY);
  if(fd==-1){
    perror("open");
    exit(1);
  }
	char* data = (char*)malloc(sizeof(char));
	while(m > 0){
    n = read(fd, data, sizeof(char));
    if(n <= 0){
      break;
    }
    m = write(s, data, sizeof(char));			    
	}
  *data='\0';
  write(s,data,sizeof(char));
 /*
  while(n = read(fd, data, sizeof(char))){
    m = write(s, data, sizeof(char));			    
	}
  */
  //printf("%d\n",1000);
	//close(s);
  close(fd);
	return 0;
}

int fax_receive(int s, char* filename){
  int m = 1; int n = 1;
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd==-1){
    perror("open");
    exit(1);
  }
  char* data = (char*)malloc(sizeof(char));
  while(m > 0){
    n = recv(s, data, sizeof(char), 0);
    if(*data=='\0') break;
    if(n <= 0){
      break;
    }
    m = write(fd, data, sizeof(char));
  }
  //close(s);
  close(fd);
  return 0;
}
