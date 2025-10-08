#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int has_upper(const char* s){for(int i=0;s[i];i++) if(s[i]>='A'&&s[i]<='Z') return 1; return 0;}
int has_lower(const char* s){for(int i=0;s[i];i++) if(s[i]>='a'&&s[i]<='z') return 1; return 0;}

int main(){
    int sock; struct sockaddr_in server;
    char in[BUFFER_SIZE], sendbuf[BUFFER_SIZE], recvbuf[BUFFER_SIZE];
    char sid[BUFFER_SIZE], user[BUFFER_SIZE], pw[BUFFER_SIZE];
    sock=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET; server.sin_port=htons(5678); 
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    connect(sock,(struct sockaddr*)&server,sizeof(server));
    printf("Connected to server.\n");

    memset(recvbuf,0,sizeof(recvbuf)); 
    recv(sock,recvbuf,sizeof(recvbuf)-1,0); printf("%s",recvbuf);
    printf(""); fgets(sid,sizeof(sid),stdin); 
    sid[strcspn(sid,"\n")]=0; send(sock,sid,strlen(sid),0);

    memset(recvbuf,0,sizeof(recvbuf)); 
    recv(sock,recvbuf,sizeof(recvbuf)-1,0);
    printf("%s",recvbuf);
    printf(""); fgets(user,sizeof(user),stdin); 
    user[strcspn(user,"\n")]=0; 
    send(sock,user,strlen(user),0);

    while(1) {
        memset(recvbuf,0,sizeof(recvbuf)); 
        recv(sock,recvbuf,sizeof(recvbuf)-1,0); 
        printf("%s",recvbuf);
        fgets(pw,sizeof(pw),stdin); pw[strcspn(pw,"\n")]=0;
        if(strlen(pw)>=6 && strlen(pw)<=15) {
            break;
        }
        printf("Invalid password length, please try again.\n");
    }
    send(sock,pw,strlen(pw),0);

    memset(recvbuf,0,sizeof(recvbuf));
    recv(sock,recvbuf,sizeof(recvbuf)-1,0);
    printf("%s",recvbuf);
    memset(recvbuf,0,sizeof(recvbuf));
    recv(sock,recvbuf,sizeof(recvbuf)-1,0);
    printf("%s",recvbuf);

    while(1) {
        char lu[BUFFER_SIZE], lp[BUFFER_SIZE];
        printf("Login Username: ");
        fgets(lu,sizeof(lu),stdin);
        lu[strcspn(lu,"\n")]=0;
        printf("Login Password (5s timeout): ");
        fflush(stdout);
        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(0,&fds);
        tv.tv_sec=5;
        tv.tv_usec=0;
        int r=select(1,&fds,NULL,NULL,&tv);
        if(r<=0){ 
            printf("\nTimeout! Please wait 10 seconds before retry.\n"); sleep(10);
            continue;
        }
        fgets(lp,sizeof(lp),stdin);
        lp[strcspn(lp,"\n")]=0;
        snprintf(sendbuf,sizeof(sendbuf),"%s|%s",lu,lp);
        send(sock,sendbuf,strlen(sendbuf),0);
        memset(recvbuf,0,sizeof(recvbuf));
        int n=recv(sock,recvbuf,sizeof(recvbuf)-1,0);
        if(n<=0) {
            return 0;
        }
        printf("%s",recvbuf);
        if(strstr(recvbuf,"success")) {
            break;
        }
    }

    while(1){
        memset(recvbuf,0,sizeof(recvbuf));
        int n=recv(sock,recvbuf,sizeof(recvbuf)-1,0);
        if(n<=0) {
            break;
        }
        printf("%s",recvbuf);
        if(strstr(recvbuf,"Select option:")==recvbuf) {
            char opt[8]; fgets(opt,sizeof(opt),stdin);
            opt[strcspn(opt,"\n")]=0;
            send(sock,opt,strlen(opt),0);
            if(opt[0]=='1') {
                memset(recvbuf,0,sizeof(recvbuf));
                recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                printf("%s",recvbuf);
                char nsid[BUFFER_SIZE];
                fgets(nsid,sizeof(nsid),stdin);
                nsid[strcspn(nsid,"\n")]=0;
                send(sock,nsid,strlen(nsid),0);
                memset(recvbuf,0,sizeof(recvbuf));
                recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                printf("%s",recvbuf);
                char nuser[BUFFER_SIZE];
                fgets(nuser,sizeof(nuser),stdin);
                nuser[strcspn(nuser,"\n")]=0;
                send(sock,nuser,strlen(nuser),0);
                while(1) {
                    memset(recvbuf,0,sizeof(recvbuf));
                    recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                    printf("%s",recvbuf);
                    char npw[BUFFER_SIZE];
                    fgets(npw,sizeof(npw),stdin);
                    npw[strcspn(npw,"\n")]=0;
                    if(strlen(npw)>=6 && strlen(npw)<=15){
                        send(sock,npw,strlen(npw),0);
                        break;
                    }
                    printf("Invalid password length, please try again.\n");
                }
                memset(recvbuf,0,sizeof(recvbuf));
                n=recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                if(n>0) {
                    printf("%s",recvbuf);
                }
            }else if(opt[0]=='2') {
                memset(recvbuf,0,sizeof(recvbuf));
                recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                printf("%s",recvbuf);
                char who[BUFFER_SIZE];
                fgets(who,sizeof(who),stdin);
                who[strcspn(who,"\n")]=0;
                send(sock,who,strlen(who),0);
                while(1) {
                    memset(recvbuf,0,sizeof(recvbuf));
                    recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                    printf("%s",recvbuf);
                    char npw[BUFFER_SIZE];
                    fgets(npw,sizeof(npw),stdin);
                    npw[strcspn(npw,"\n")]=0;
                    int ok = (strlen(npw)>=12 && strlen(npw)<=20 && has_upper(npw) && has_lower(npw));
                    if(ok) {
                        send(sock,npw,strlen(npw),0);
                        break;
                    }
                    printf("Please enter the new password again.\n");
                }
                memset(recvbuf,0,sizeof(recvbuf));
                n=recv(sock,recvbuf,sizeof(recvbuf)-1,0);
                if(n>0) {
                    printf("%s",recvbuf);
                }
            }else if(opt[0]=='3') {
                break;
            }
        }
    }
    close(sock);
    return 0;
}
