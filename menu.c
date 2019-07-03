#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>

const int N_size = 1<<15;

//prottype...
int phone_server(int s);
int phone_client(int s,struct sockaddr *addr);
int record_server(int s, char *filename);
int record_client(int s, char *filename);
int fax_send(int s,char *filename);
int fax_receive(int s,char *filename);

int phone_main_server(int s);
void *phone_func( void * arg );
//void add();

void print_order(int s){
    write(s,"----select order from below----\nphone\nfax_send\nfax_receive\nrecord\nadd\nquit\n-------------------------------\n\n",109);
}

int main(int argc, char **argv) {
    if(argc<2) {
        perror("too few input");
        return 0;
    }
    else if(argc>3){
        perror("too many input");
        return 0;
    }

    //server側の処理
    if(argc==2){
        char *port;
        port = (char *)malloc(sizeof(char)*30);
        strcpy(port,argv[1]);
        int port_i = atoi(port);

        char data[N_size];
        int ss = socket(PF_INET,SOCK_STREAM,0);
        
        int port_ct=1;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_i);
        addr.sin_addr.s_addr = INADDR_ANY;
        bind(ss, (struct sockaddr *)&addr, sizeof(addr));
        listen(ss,10);

        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int s = accept(ss,(struct sockaddr *)&client_addr, &len);

        if(s>0)printf("connect.\n");
        close(ss);
        while(1){
            print_order(s);
            memset(data,'\0',N_size);
            printf("client choose an order...\n\n");
            read(s,data,N_size);
            //opはclientに送る命令番号
            int *op=(int *)malloc(sizeof(int));
            //ansはY/N判定
            char *ans=(char *)malloc(sizeof(char));
            //dumpはEnterキーの墓場
            char *dump=(char *)malloc(sizeof(char));
            //dataには改行まで含まれているのでそれより前で比較
            if(memcmp(data,"phone",5)==0){
                printf("request %sselect <Y/N>\n",data);
                read(0,ans,sizeof(char));
                read(0,dump,sizeof(char));
                *op=(*ans=='Y'?1:8);
                //*op=1;
                write(s,op,sizeof(int));
                if(*ans=='Y'){
                    phone_main_server(s);
                        
                    struct sockaddr_in addr2;
                    addr2.sin_family = AF_INET;
                    addr2.sin_port = htons(port_i+port_ct);
                    port_ct++;
                    addr2.sin_addr.s_addr = INADDR_ANY;
                    int ss = socket(PF_INET,SOCK_STREAM,0);
                    bind(ss, (struct sockaddr *)&addr2, sizeof(addr2));
                    listen(ss,10);
                    s = accept(ss,(struct sockaddr *)&client_addr, &len);

                    if(s>0)printf("connect.\n");
                    else perror("fail to connect\n");
                    close(ss);
                }
            }
            else if(memcmp(data,"fax_send",8)==0){
                printf("request %sselect <Y/N>\n",data);
                read(0,ans,sizeof(char));
                read(0,dump,sizeof(char));
                *op=(*ans=='Y'?2:8);
                //*op=2;
                if(*ans=='Y'){
                    char *filename=(char *)malloc(sizeof(char)*100);
                    printf("please input filename to recive.\n");
                    scanf("%s",filename);
                    write(s,op,sizeof(int));
                    fax_receive(s,filename);
                    free(filename);
                }
                else write(s,op,sizeof(int));
            }
            else if(memcmp(data,"fax_receive",11)==0){
                printf("request %sselect <Y/N>\n",data);
                read(0,ans,sizeof(char));
                read(0,dump,sizeof(char));
                *op=(*ans=='Y'?7:8);
                //*op=2;
                if(*ans=='Y'){
                    char *filename=(char *)malloc(sizeof(char)*100);
                    printf("please input filename to send.\n");
                    scanf("%s",filename);
                    write(s,op,sizeof(int));
                    usleep(10000);
                    fax_send(s,filename);
                    free(filename);
                }
                else write(s,op,sizeof(int));
            }
            else if(memcmp(data,"record",6)==0){
                printf("request %sselect <Y/N>\n",data);
                read(0,ans,sizeof(char));
                read(0,dump,sizeof(char));
                printf("ans = %c\n\n",*ans);
                *op=(*ans=='Y'?3:8);
                //*op=3;
                write(s,op,sizeof(int));
                //if(*ans=='Y')record_server(s, "recorder.raw"); // 録音するファイル名(new)
                if(*ans=='Y'){
                    char *filename=(char *)malloc(sizeof(char)*100);
                    printf("please input filename to record.\n");
                    scanf("%s",filename);
                    write(s,op,sizeof(int));
                    usleep(10000);
                    fax_send(s,filename);
                    free(filename);
                }
                else write(s,op,sizeof(int));
            }
            else if(memcmp(data,"add",3)==0){
                printf("request %sselect <Y/N>\n",data);
                read(0,ans,sizeof(char));
                read(0,dump,sizeof(char));
                *op=(*ans=='Y'?4:8);
                //*op=4;
                write(s,op,sizeof(int));
                //add();
            }
            else if(memcmp(data,"quit",4)==0){
                printf("client select quit.\n");
                //*op=(*ans=='Y'?5:8);
                *op=5;
                write(s,op,sizeof(int));
                //if(*ans=='Y')
                break;
            }
            else{
                *op=6;
                write(s,op,sizeof(int));
            }
            //usleep(10000);
            if(*op!=6 && *op!=8)printf("finish!\n\n");
            else if(*op==8) printf("reject client's request\n\n");
            else printf("invalid requese\n\n");
            free(op);
            free(ans);
            free(dump);
            usleep(500000);
        }
    close(s);
    }

    //client側の処理
    else if(argc==3){
        char *ip,*port;
        ip=(char *)malloc(sizeof(char)*30);
        port = (char *)malloc(sizeof(char)*30);
        strcpy(ip,argv[1]);
        strcpy(port,argv[2]);
        int port_i = atoi(port);

        int s = socket(PF_INET,SOCK_STREAM,0);

        int port_ct=1;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        int jg = inet_aton(ip,&addr.sin_addr);
        if(jg==0){
            perror("cannot find ip.");
            return 0;
        }

        addr.sin_port = htons(port_i);
        int ret = connect(s,(struct sockaddr *)&addr, sizeof(addr));
        
        if(ret!=0){
            perror("cannot connect.");
            return 0;
        }

        char *data=(char *)malloc(sizeof(char)*N_size);
        int n;

        while(1){
            n = recv(s,data,N_size,0);
            write(1,data,n);
            memset(data,'\0',N_size);

            int read_ct=read(0,data,N_size);
            char *filename=(char *)malloc(sizeof(char)*100);
            if(memcmp(data,"fax_receive",11)==0){
                printf("please input filename to receive\n");
                scanf("%s",filename);
            }
            else if(memcmp(data,"fax_send",8)==0){
                printf("please input filename to send\n");
                scanf("%s",filename);
            }
            else if(memcmp(data,"record",8)==0){
                printf("please input filename to record\n");
                scanf("%s",filename);
            }


            write(s,data,read_ct);

            int *op=(int *)malloc(sizeof(int));
            recv(s,op,sizeof(op),0);
            
            bool quit_jg=false;
            //int flag = 0;
            switch(*op){
                case 1:
                    //phone
                    printf("request is accepted!\n\n");
                    phone_client(s,(struct sockaddr *)&addr);
                    
                    s = socket(PF_INET,SOCK_STREAM,0);
                    struct sockaddr_in addr;
                    addr.sin_family = AF_INET;
                    int jg = inet_aton(ip,&addr.sin_addr);
                    if(jg==0){
                        perror("cannot find ip.");
                        return 0;
                    }

                    addr.sin_port = htons(port_i+port_ct);
                    port_ct++;
                    sleep(5);
                    int ret = connect(s,(struct sockaddr *)&addr, sizeof(addr));
                    
                    if(ret!=0){
                        perror("cannot connect.\n");
                        return 0;
                    }
                    else printf("connect!\n\n");

                    break;
                case 2:
                    //fax_send
                    printf("request is accepted!\n\n");
                    fax_send(s,filename);
                    break;
                case 7:
                    //fax_receive
                    printf("request is accepted!\n\n");
                    //fax_receive(s, "filename.txt");
                    fax_receive(s,filename);
                    break;
                case 3:
                    printf("request is accepted!\n\n");
                    record_client(s, filename);
                    break;
                case 4:
                    //add
                    printf("request is accepted!\n\n");
                    write(0,"add\n",5);
                    break;
                case 5:
                    //quit
                    quit_jg=true;
                    break;
                case 6:
                    //out of order
                    write(0,"out of order\n\n",14);
                    //先頭に戻る
                    break;
                case 8:
                    //reject
                    write(0,"reject your request\n\n",21);
                    break;
                default:
                    break;
            }
            free(op);
            free(filename);
            if(quit_jg) break;
        }
        close(s);
        return 0;
    }
}