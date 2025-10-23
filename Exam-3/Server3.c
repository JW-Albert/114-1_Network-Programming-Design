#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define MAX_ROOMS 5
#define BUFFER_SIZE 1024

typedef struct{
    int sockets[MAX_CLIENTS];
    char names[MAX_CLIENTS][20];
    char roomName[20];
    int count;
}Room;

int client_sockets[MAX_CLIENTS];
char client_names[MAX_CLIENTS][20];
Room rooms[MAX_ROOMS];
pthread_mutex_t lock;

void broadcast_all(const char *msg){
    for(int i=0;i<MAX_CLIENTS;i++)
        if(client_sockets[i]!=0)
            send(client_sockets[i],msg,strlen(msg),0);
}

void broadcast_room(Room *r,const char *msg){
    for(int i=0;i<r->count;i++)
        send(r->sockets[i],msg,strlen(msg),0);
}

Room* find_room(const char *name){
    for(int i=0;i<MAX_ROOMS;i++)
        if(strcmp(rooms[i].roomName,name)==0)return &rooms[i];
    return NULL;
}

void remove_from_room(Room *r,int sock){
    for(int i=0;i<r->count;i++)
        if(r->sockets[i]==sock){
            for(int j=i;j<r->count-1;j++){
                r->sockets[j]=r->sockets[j+1];
                strcpy(r->names[j],r->names[j+1]);
            }
            r->count--;
            break;
        }
}

void *client_handler(void *arg){
    int sock=*(int*)arg,idx=-1;
    char buf[BUFFER_SIZE];
    Room *current=find_room("Lobby");
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_CLIENTS;i++)
        if(client_sockets[i]==sock)idx=i;
    if(!current){
        for(int i=0;i<MAX_ROOMS;i++)
            if(rooms[i].count==0){strcpy(rooms[i].roomName,"Lobby");current=&rooms[i];break;}
    }
    current->sockets[current->count]=sock;
    sprintf(current->names[current->count],"%d",idx+1);
    current->count++;
    pthread_mutex_unlock(&lock);
    sprintf(buf,"[Server]: Client%d joined Lobby.\n",idx+1);
    broadcast_room(current,buf);
    while(1){
        memset(buf,0,sizeof(buf));
        int len=recv(sock,buf,sizeof(buf)-1,0);
        if(len<=0)break;
        buf[len]='\0';
        if(strncmp(buf,"CREATE ",7)==0){
            char roomName[20];int t1,t2;
            sscanf(buf+7,"%s %d %d",roomName,&t1,&t2);
            pthread_mutex_lock(&lock);
            Room *r=find_room(roomName);
            if(!r){
                for(int i=0;i<MAX_ROOMS;i++)
                    if(rooms[i].count==0){strcpy(rooms[i].roomName,roomName);r=&rooms[i];break;}
            }
            pthread_mutex_unlock(&lock);
            if(!r){send(sock,"Room full!\n",11,0);continue;}
            char msg[BUFFER_SIZE];
            sprintf(msg,"[Server]: You invited Client%d and Client%d to %s.\n",t1,t2,roomName);
            send(sock,msg,strlen(msg),0);
            char inv[BUFFER_SIZE];
            sprintf(inv,"INVITE %s %d\n",roomName,idx+1);
            if(t1>=1&&t1<=MAX_CLIENTS&&client_sockets[t1-1]!=0)
                send(client_sockets[t1-1],inv,strlen(inv),0);
            if(t2>=1&&t2<=MAX_CLIENTS&&client_sockets[t2-1]!=0)
                send(client_sockets[t2-1],inv,strlen(inv),0);
        }else if(strncmp(buf,"JOIN ",5)==0){
            char roomName[20];sscanf(buf+5,"%s",roomName);
            Room *r=find_room(roomName);
            if(!r){send(sock,"invalid room!\n",14,0);continue;}
            if(r->count>=3){send(sock,"room full!\n",11,0);continue;}
            pthread_mutex_lock(&lock);
            r->sockets[r->count]=sock;
            sprintf(r->names[r->count],"%d",idx+1);
            r->count++;
            pthread_mutex_unlock(&lock);
            char msg[BUFFER_SIZE];
            sprintf(msg,"[Server]: Client%d joined %s.\n",idx+1,roomName);
            broadcast_room(r,msg);
        }else if(strncmp(buf,"MSG ",4)==0){
            char msg[BUFFER_SIZE];
            sprintf(msg,"[Client%d]: %s\n",idx+1,buf+4);
            broadcast_room(current,msg);
        }else if(strncmp(buf,"MOVE ",5)==0){
            char roomName[20];sscanf(buf+5,"%s",roomName);
            Room *r=find_room(roomName);
            if(!r){send(sock,"invalid room!\n",14,0);continue;}
            pthread_mutex_lock(&lock);
            remove_from_room(current,sock);
            r->sockets[r->count]=sock;
            sprintf(r->names[r->count],"%d",idx+1);
            r->count++;
            current=r;
            pthread_mutex_unlock(&lock);
            char msg[BUFFER_SIZE];
            sprintf(msg,"[Server]: Client%d moved to %s.\n",idx+1,roomName);
            broadcast_room(r,msg);
        }else if(strcmp(buf,"Exit")==0){
            sprintf(buf,"[Server]: Client%d disconnected.\n",idx+1);
            broadcast_all(buf);
            break;
        }else send(sock,"invalid command!\n",18,0);
    }
    close(sock);
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_ROOMS;i++)remove_from_room(&rooms[i],sock);
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
        if(i==MAX_CLIENTS){send(new_sock,"Server is full!\n",16,0);close(new_sock);}
        pthread_mutex_unlock(&lock);
    }
    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
