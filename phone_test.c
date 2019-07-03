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
#include <fcntl.h>
#include <string.h>

#define N 1000

void *phone_func( void * arg );
int phone_server(int s);
int phone_client(int s,struct sockaddr *addr);

int phone_main_server(int s){
    pthread_t pt;
    char      line[N];
    bool loop_jg=true;
    int ct=0;
    printf("please use cmd 'exit' to stop this call...\n");
    while(loop_jg) {
        if(ct!=0){
            fgets(line, sizeof(line), stdin );
            if(strlen( line ) <= 1) {
                continue;
            }
            if(memcmp( line, "exit", 4 ) == 0) {
                loop_jg=false;
                break;
            }
            else continue;
        }
        else{
            pthread_create( &pt, NULL, &phone_func, &s);
            ct++;
        }
    }
    close(s);
    sleep(3);
    return 0;
}

void *phone_func(void *arg){
    int s =*(int *)arg;
    pthread_detach(pthread_self( ));
    int ns=1; int nr = 1;
    FILE *fp_read; FILE *fp_write;
    int flag_read = 0; int flag_write = 0;
    char* data_send = (char*)malloc(N * sizeof(char));
    char* data_recv = (char*)malloc(N * sizeof(char));
 	while(ns > 0){
        if(flag_write==0){
            if((fp_read = popen("rec -t raw -q -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
            flag_write=1;
        }
        
        ns = fread(data_send, sizeof(char), N * sizeof(char), fp_read);
        ns = send(s, data_send, N * sizeof(char), 0);
        //while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0) continue;
        nr = recv(s, data_recv, N*sizeof(char), 0);
        if(flag_read==0){
            if((fp_write = popen("play -t raw -q -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
            flag_read=1;
        }
        nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
    }
    pclose(fp_read);
    pclose(fp_write);
	return 0;
}

int phone_client(int s,struct sockaddr *addr){
  int ns=1; int nr = 1;
  FILE *fp_read; FILE *fp_write;
  int flag_read = 0; int flag_write = 0;
  char* data_send = (char*)malloc(N * sizeof(char));
  char* data_recv = (char*)malloc(N * sizeof(char));
  int n;
  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  fd_set fds, readfds;
  FD_SET(s, &readfds);
    while(ns > 0){
        memcpy(&fds, &readfds, sizeof(fd_set));

        /* fdsに設定されたソケットが読み込み可能になるまで待ちます */
        n = select(s+1, &fds, NULL, NULL, &tv);

        /* タイムアウトの場合にselectは0を返します */
        if (n == 0) {
            /* ループから抜けます */
            printf("server quit this phone....\n\n");
            break;
        }
        //while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0)
        nr = recv(s, data_recv, N*sizeof(char), 0);
            //continue;
            //break;
            //int ret = connect(s,(struct sockaddr *)&addr, sizeof(addr));
            /*
            if(ret!=0){
                printf("cannot connect.");
                break;
            }
            */
        //printf("%d\n",ns);
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
        //printf("sock = %d\n",ns);
    }
    //close(s);
    pclose(fp_read);
    pclose(fp_write);
    return 0;
}



int phone_server(int s){
    int ns=1; int nr = 1;
    FILE *fp_read; FILE *fp_write;
    int flag_read = 0; int flag_write = 0;
    char* data_send = (char*)malloc(N * sizeof(char));
    char* data_recv = (char*)malloc(N * sizeof(char));
 	while(ns > 0){
        if(flag_write==0){
            if((fp_read = popen("rec -t raw -q -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) return 1;
            flag_write=1;
        }
        ns = fread(data_send, sizeof(char), N * sizeof(char), fp_read);
        ns = send(s, data_send, N * sizeof(char), 0);
        while((nr = recv(s, data_recv, N*sizeof(char), 0)) < 0) {
            continue;
        }
        if(nr == 0){
        //break;
        }
        if(flag_read==0){
            if((fp_write = popen("play -t raw -q -b 16 -c 1 -e s -r 44100 -", "w"))==NULL) return 1;
            flag_read=1;
        }
        nr = fwrite(data_recv, 1, N * sizeof(char), fp_write);
    }

	close(s);
    pclose(fp_read);
    pclose(fp_write);
	return 0;
}