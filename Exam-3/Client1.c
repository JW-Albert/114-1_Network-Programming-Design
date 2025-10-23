#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;

void *recv_handler(void *arg) {
    char msg[BUFFER_SIZE];
    while (1) {
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
    printf("Commands:\n1) BROADCAST <msg>\n2) SEND <SockID> <msg>\n3) EXIT\n");
    pthread_create(&tid,NULL,recv_handler,NULL);
    char input[BUFFER_SIZE];
    while(1){
        memset(input,0,sizeof(input));
        fgets(input,sizeof(input),stdin);
        input[strcspn(input,"\n")]=0;
        if(strncmp(input,"BROADCAST ",10)==0){
            int len=strlen(input+10);
            if(len<10||len>15){printf("invalid content!\n");continue;}
            send(sock,input,strlen(input),0);
        }else if(strncmp(input,"SEND ",5)==0||strcmp(input,"EXIT")==0){
            send(sock,input,strlen(input),0);
            if(strcmp(input,"EXIT")==0)break;
        }else printf("invalid command!\n");
    }
    close(sock);
    pthread_cancel(tid);
    pthread_join(tid,NULL);
    return 0;
}
