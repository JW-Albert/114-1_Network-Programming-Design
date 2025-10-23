#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
#define MAX_USERS 100

typedef struct {
    char sid[20];
    char user[20];
    char pass[20];
} User;

int client_sockets[MAX_CLIENTS];
User users[MAX_USERS];
int user_count=0;
pthread_mutex_t lock;

int valid_password(const char *p){
    int up=0,sym=0;
    for(int i=0;p[i];i++){
        if(p[i]>='A'&&p[i]<='Z')up=1;
        else if(!(p[i]>='a'&&p[i]<='z')&&!(p[i]>='0'&&p[i]<='9'))sym=1;
    }
    return up&&sym;
}

int find_user(const char *u){
    for(int i=0;i<user_count;i++)
        if(strcmp(users[i].user,u)==0)return i;
    return -1;
}

void *client_handler(void *arg){
    int sock=*(int*)arg,idx=-1;
    char buf[BUFFER_SIZE];
    int logged=0;
    char sid[20];
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_CLIENTS;i++)
        if(client_sockets[i]==sock)idx=i;
    pthread_mutex_unlock(&lock);
    while(1){
        memset(buf,0,sizeof(buf));
        int len=recv(sock,buf,sizeof(buf)-1,0);
        if(len<=0)break;
        buf[len]='\0';
        if(strncmp(buf,"SIGNUP ",7)==0){
            char s[20],u[20],p[20];
            sscanf(buf+7,"%s %s %s",s,u,p);
            if(strlen(u)<8||strlen(u)>15||strlen(p)<8||strlen(p)>15||!valid_password(p)){
                send(sock,"invalid signup!\n",16,0);
                continue;
            }
            if(find_user(u)!=-1){
                send(sock,"user exists!\n",13,0);
                continue;
            }
            pthread_mutex_lock(&lock);
            strcpy(users[user_count].sid,s);
            strcpy(users[user_count].user,u);
            strcpy(users[user_count].pass,p);
            user_count++;
            pthread_mutex_unlock(&lock);
            send(sock,"signup success!\n",16,0);
        }else if(strncmp(buf,"LOGIN ",6)==0){
            char u[20],p[20];
            sscanf(buf+6,"%s %s",u,p);
            int id=find_user(u);
            if(id==-1||strcmp(users[id].pass,p)!=0){
                send(sock,"login failed!\n",14,0);
                continue;
            }
            logged=1;
            strcpy(sid,users[id].sid);
            send(sock,"login success!\n",15,0);
        }else if(strcmp(buf,"EXIT")==0)break;
        else if(logged){
            char out[BUFFER_SIZE];
            sprintf(out,"[%s]: %s\n",sid,buf);
            pthread_mutex_lock(&lock);
            for(int i=0;i<MAX_CLIENTS;i++)
                if(client_sockets[i]!=0)
                    send(client_sockets[i],out,strlen(out),0);
            pthread_mutex_unlock(&lock);
        }else send(sock,"please login first!\n",21,0);
    }
    close(sock);
    pthread_mutex_lock(&lock);
    client_sockets[idx]=0;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(){
    int server_fd,new_sock;
    struct sockaddr_in saddr,caddr;
    socklen_t clen=sizeof(caddr);
    pthread_t tid;
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(PORT);
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bind(server_fd,(struct sockaddr*)&saddr,sizeof(saddr));
    listen(server_fd,5);
    pthread_mutex_init(&lock,NULL);
    while(1){
        new_sock=accept(server_fd,(struct sockaddr*)&caddr,&clen);
        pthread_mutex_lock(&lock);
        int i;
        for(i=0;i<MAX_CLIENTS;i++)
            if(client_sockets[i]==0){
                client_sockets[i]=new_sock;
                pthread_create(&tid,NULL,client_handler,&new_sock);
                pthread_detach(tid);
                break;
            }
        if(i==MAX_CLIENTS){
            send(new_sock,"Server is full!\n",16,0);
            close(new_sock);
        }
        pthread_mutex_unlock(&lock);
    }
    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
