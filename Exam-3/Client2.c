#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;

void *recv_handler(void *arg){
    char msg[BUFFER_SIZE];
    while(1){
        memset(msg,0,sizeof(msg));
        int len=recv(sock,msg,sizeof(msg)-1,0);
        if(len<=0)break;
        msg[len]='\0';
        printf("%s",msg);
        fflush(stdout);
    }
    pthread_exit(NULL);
}

int main(){
    struct sockaddr_in saddr;
    pthread_t tid;
    sock=socket(AF_INET,SOCK_STREAM,0);
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(PORT);
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(sock,(struct sockaddr*)&saddr,sizeof(saddr))<0){
        perror("connect");return 1;
    }
    pthread_create(&tid,NULL,recv_handler,NULL);
    printf("Commands:\nSIGNUP <studentID> <user> <pass>\nLOGIN <user> <pass>\nEXIT\n");
    char input[BUFFER_SIZE];
    while(1){
        memset(input,0,sizeof(input));
        fgets(input,sizeof(input),stdin);
        input[strcspn(input,"\n")]=0;
        send(sock,input,strlen(input),0);
        if(strcmp(input,"EXIT")==0)break;
    }
    close(sock);
    pthread_cancel(tid);
    pthread_join(tid,NULL);
    return 0;
}
