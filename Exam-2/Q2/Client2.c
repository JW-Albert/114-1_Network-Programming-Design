#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server;
    char buf[BUFFER_SIZE], recvbuf[BUFFER_SIZE];

    sock=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(5678);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    connect(sock,(struct sockaddr*)&server,sizeof(server));

    while(1){
        int len=recv(sock,recvbuf,sizeof(recvbuf)-1,0);
        if(len<=0) break;
        recvbuf[len]='\0';
        printf("%s",recvbuf);
        fgets(buf,sizeof(buf),stdin);
        buf[strcspn(buf,"\n")]=0;
        send(sock,buf,strlen(buf),0);
    }

    close(sock);
    return 0;
}
