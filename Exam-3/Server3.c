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
pthread_mutex_t lock;
Room rooms[MAX_ROOMS];

Room* find_room(const char *name){
    for(int i=0;i<MAX_ROOMS;i++)
        if(strcmp(rooms[i].roomName,name)==0)return &rooms[i];
    return NULL;
}

void broadcast_room(Room *r,const char *msg){
    for(int i=0;i<r->count;i++)
        send(r->sockets[i],msg,strlen(msg),0);
}

void broadcast_all(const char *msg){
    for(int i=0;i<MAX_CLIENTS;i++)
        if(client_sockets[i]!=0)
            send(client_sockets[i],msg,strlen(msg),0);
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

Room* find_room_by_sock(int sock){
    for(int i=0;i<MAX_ROOMS;i++)
        for(int j=0;j<rooms[i].count;j++)
            if(rooms[i].sockets[j]==sock)return &rooms[i];
    return NULL;
}

void *client_handler(void *arg){
    int sock=*(int*)arg,idx=-1;
    char buf[BUFFER_SIZE];
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_CLIENTS;i++)
        if(client_sockets[i]==sock)idx=i;
    Room *lobby=find_room("Lobby");
    if(!lobby){
        for(int i=0;i<MAX_ROOMS;i++)
            if(rooms[i].count==0){strcpy(rooms[i].roomName,"Lobby");lobby=&rooms[i];break;}
    }
    lobby->sockets[lobby->count]=sock;
    sprintf(lobby->names[lobby->count],"%d",idx+1);
    lobby->count++;
    pthread_mutex_unlock(&lock);
    sprintf(buf,"[Server]: Client%d joined Lobby.\n",idx+1);
    broadcast_room(lobby,buf);
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
            if(!r){pthread_mutex_unlock(&lock);send(sock,"Room full!\n",11,0);continue;}
            remove_from_room(lobby,sock);
            r->sockets[r->count]=sock;
            sprintf(r->names[r->count],"%d",idx+1);
            r->count++;
            pthread_mutex_unlock(&lock);
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
            Room *cur=find_room_by_sock(sock);
            if(cur)remove_from_room(cur,sock);
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
            Room *r=find_room_by_sock(sock);
            if(r)broadcast_room(r,msg);
        }else if(strncmp(buf,"MOVE ",5)==0){
            char roomName[20];sscanf(buf+5,"%s",roomName);
            Room *r=find_room(roomName);
            if(!r){send(sock,"invalid room!\n",14,0);continue;}
            pthread_mutex_lock(&lock);
            Room *cur=find_room_by_sock(sock);
            if(cur)remove_from_room(cur,sock);
            r->sockets[r->count]=sock;
            sprintf(r->names[r->count],"%d",idx+1);
            r->count++;
            pthread_mutex_unlock(&lock);
            char msg[BUFFER_SIZE];
            sprintf(msg,"[Server]: Client%d moved to %s.\n",idx+1,roomName);
            broadcast_room(r,msg);
        }else if(strcmp(buf,"LEAVE")==0){
            pthread_mutex_lock(&lock);
            Room *cur=find_room_by_sock(sock);
            if(cur)remove_from_room(cur,sock);
            Room *l=find_room("Lobby");
            if(!l){
                for(int i=0;i<MAX_ROOMS;i++)
                    if(rooms[i].count==0){strcpy(rooms[i].roomName,"Lobby");l=&rooms[i];break;}
            }
            l->sockets[l->count]=sock;
            sprintf(l->names[l->count],"%d",idx+1);
            l->count++;
            pthread_mutex_unlock(&lock);
            sprintf(buf,"[Server]: Client%d returned to Lobby.\n",idx+1);
            broadcast_room(l,buf);
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
