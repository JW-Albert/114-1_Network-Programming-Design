#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int timedInput(char *buf,int sec){
    fd_set fds; struct timeval t; FD_ZERO(&fds);
    FD_SET(0,&fds); t.tv_sec=sec; t.tv_usec=0;
    int r=select(1,&fds,NULL,NULL,&t);
    if(r==0) return 0;
    fgets(buf,BUFFER_SIZE,stdin);
    buf[strcspn(buf,"\n")]=0;
    return 1;
}

int main(){
    int sock; struct sockaddr_in server;
    char buf[BUFFER_SIZE], sendbuf[BUFFER_SIZE];
    sock=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(5678);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    connect(sock,(struct sockaddr*)&server,sizeof(server));
    while(1){
        printf("Select Function (A/B/C): ");
        fgets(buf,sizeof(buf),stdin);
        buf[strcspn(buf,"\n")]=0;
        send(sock,buf,strlen(buf),0);
        if(strcmp(buf,"A")==0){
            char a[BUFFER_SIZE], b[BUFFER_SIZE], op[BUFFER_SIZE];
            printf("Enter A (5s): "); if(!timedInput(a,5)) strcpy(a,"-1");
            printf("Enter B (5s): "); if(!timedInput(b,5)) strcpy(b,"-1");
            printf("Operator (+-*/): "); fgets(op,sizeof(op),stdin); op[strcspn(op,"\n")]=0;
            send(sock,a,strlen(a),0); usleep(100000);
            send(sock,b,strlen(b),0); usleep(100000);
            send(sock,op,strlen(op),0);
            int len=recv(sock,buf,sizeof(buf)-1,0); buf[len]=0; printf("%s",buf);
        } else if(strcmp(buf,"B")==0){
            int count=0;
            while(count<10){
                printf("Enter number %d (3s): ",count+1);
                if(!timedInput(sendbuf,3)){ strcpy(sendbuf,"0"); send(sock,sendbuf,strlen(sendbuf),0); break; }
                if(strlen(sendbuf)==0) break;
                send(sock,sendbuf,strlen(sendbuf),0);
                count++;
                if(count>=4){ printf("Continue? (y/n): "); char c[8]; fgets(c,8,stdin); if(c[0]=='n'||c[0]=='N') break; }
            }
            send(sock,"END",3,0);
            int len=recv(sock,buf,sizeof(buf)-1,0); buf[len]=0; printf("%s",buf);
        } else if(strcmp(buf,"C")==0){
            char data[BUFFER_SIZE]="", tmp[BUFFER_SIZE];
            printf("Start typing letters (10s): ");
            fd_set fds; struct timeval t; FD_ZERO(&fds); FD_SET(0,&fds);
            t.tv_sec=10; t.tv_usec=0;
            while(1){
                int r=select(1,&fds,NULL,NULL,&t);
                if(r==0) break;
                fgets(tmp,sizeof(tmp),stdin);
                strcat(data,tmp);
            }
            send(sock,data,strlen(data),0);
            int len=recv(sock,buf,sizeof(buf)-1,0); buf[len]=0; printf("%s",buf);
        }
    }
    close(sock);
    return 0;
}
