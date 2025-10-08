#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>

#define MAX_USER 3
#define BUFFER_SIZE 256

struct Account {
    char id[BUFFER_SIZE];
    char user[BUFFER_SIZE];
    char pass[BUFFER_SIZE];
};

int checkPasswordRule(const char *p) {
    int upper=0, lower=0;
    if (strlen(p)<12 || strlen(p)>20) return 0;
    for (int i=0;p[i];i++){
        if (isupper(p[i])) upper=1;
        if (islower(p[i])) lower=1;
    }
    return upper && lower;
}

int main() {
    int sock, csock, readsize, addrlen, userCount=0;
    struct sockaddr_in server, client;
    struct Account acc[MAX_USER];
    char buf[BUFFER_SIZE], buf2[BUFFER_SIZE];
    fd_set readfds; struct timeval timeout;

    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(5678);
    server.sin_addr.s_addr=INADDR_ANY;

    sock=socket(AF_INET,SOCK_STREAM,0);
    bind(sock,(struct sockaddr*)&server,sizeof(server));
    listen(sock,5);
    addrlen=sizeof(client);
    csock=accept(sock,(struct sockaddr*)&client,&addrlen);

    send(csock,"Register ID:",12,0);
    recv(csock,acc[userCount].id,sizeof(acc[userCount].id),0);
    send(csock,"Register Username:",18,0);
    recv(csock,acc[userCount].user,sizeof(acc[userCount].user),0);
    send(csock,"Register Password:",18,0);
    recv(csock,acc[userCount].pass,sizeof(acc[userCount].pass),0);
    userCount++;
    send(csock,"Registration done.",18,0);

    while(1){
        send(csock,"Please login ID:",16,0);
        recv(csock,buf,sizeof(buf),0);
        send(csock,"Please login Password:",23,0);
        FD_ZERO(&readfds);
        FD_SET(csock,&readfds);
        timeout.tv_sec=5; timeout.tv_usec=0;
        int ret=select(csock+1,&readfds,NULL,NULL,&timeout);
        if(ret==0){ send(csock,"Timeout. Wait 10s.\n",19,0); sleep(10); continue; }
        recv(csock,buf2,sizeof(buf2),0);
        int found=-1;
        for(int i=0;i<userCount;i++)
            if(strcmp(acc[i].id,buf)==0){ found=i; break; }
        if(found==-1){ send(csock,"Wrong ID!!!\n",12,0); continue; }
        if(strcmp(acc[found].pass,buf2)!=0){ send(csock,"Wrong Password!!!\n",18,0); sleep(10); continue; }
        send(csock,"Login success!\n",15,0);
        break;
    }

    while(1){
        send(csock,"Select: 1.New Account 2.Change Password\n",40,0);
        recv(csock,buf,sizeof(buf),0);
        if(strcmp(buf,"1")==0){
            if(userCount>=MAX_USER){ send(csock,"Account full\n",13,0); continue; }
            send(csock,"New ID:",7,0);
            recv(csock,buf,sizeof(buf),0);
            int dup=0;
            for(int i=0;i<userCount;i++)
                if(strcmp(acc[i].id,buf)==0){ dup=1; break; }
            if(dup){ send(csock,"Duplicate ID. Reject.\n",22,0); continue; }
            strcpy(acc[userCount].id,buf);
            send(csock,"New Username:",13,0);
            recv(csock,acc[userCount].user,sizeof(acc[userCount].user),0);
            send(csock,"New Password:",13,0);
            recv(csock,acc[userCount].pass,sizeof(acc[userCount].pass),0);
            userCount++;
            send(csock,"New account created.\n",21,0);
        } else if(strcmp(buf,"2")==0){
            send(csock,"Enter ID:",9,0);
            recv(csock,buf,sizeof(buf),0);
            int idx=-1;
            for(int i=0;i<userCount;i++)
                if(strcmp(acc[i].id,buf)==0){ idx=i; break; }
            if(idx==-1){ send(csock,"No such ID\n",11,0); continue; }
            send(csock,"Enter new password:",19,0);
            recv(csock,buf2,sizeof(buf2),0);
            if(!checkPasswordRule(buf2)){ send(csock,"Please enter the new password again.\n",36,0); continue; }
            strcpy(acc[idx].pass,buf2);
            send(csock,"Password updated.\n",18,0);
        }
    }

    close(csock);
    close(sock);
    return 0;
}
