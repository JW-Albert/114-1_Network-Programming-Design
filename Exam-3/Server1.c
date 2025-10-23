#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
time_t last_broadcast[MAX_CLIENTS];
pthread_mutex_t lock;

void send_to_client(int target, const char *msg) {
    if (client_sockets[target] != 0)
        send(client_sockets[target], msg, strlen(msg), 0);
}

void broadcast_message(const char *msg) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (client_sockets[i] != 0)
            send(client_sockets[i], msg, strlen(msg), 0);
}

void *client_handler(void *arg) {
    int sock = *(int*)arg, idx = -1;
    char buffer[BUFFER_SIZE];
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (client_sockets[i] == sock) idx = i;
    pthread_mutex_unlock(&lock);
    sprintf(buffer, "[Server] Client%d joined.\n", idx+1);
    broadcast_message(buffer);
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        if (strncmp(buffer, "BROADCAST ", 10) == 0) {
            time_t now = time(NULL);
            if (difftime(now, last_broadcast[idx]) < 5) {
                send(sock, "broadcast request denied!\n", 27, 0);
                continue;
            }
            last_broadcast[idx] = now;
            char msg[BUFFER_SIZE+50];
            sprintf(msg, "[Client%d]: %s\n", idx+1, buffer+10);
            broadcast_message(msg);
        } else if (strncmp(buffer, "SEND ", 5) == 0) {
            int target;
            char msg[BUFFER_SIZE];
            sscanf(buffer+5, "%d %[^\n]", &target, msg);
            if (target<1 || target>MAX_CLIENTS || client_sockets[target-1]==0) {
                send(sock, "invalid target!\n", 16, 0);
                continue;
            }
            char out[BUFFER_SIZE+50];
            sprintf(out, "[Client%d->Client%d]: %s\n", idx+1, target, msg);
            send_to_client(target-1, out);
        } else if (strcmp(buffer, "EXIT") == 0) {
            sprintf(buffer, "[Server] Client%d left.\n", idx+1);
            broadcast_message(buffer);
            break;
        } else send(sock, "invalid command!\n", 18, 0);
    }
    close(sock);
    pthread_mutex_lock(&lock);
    client_sockets[idx] = 0;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in saddr, caddr;
    socklen_t clen = sizeof(caddr);
    pthread_t tid;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(server_fd, (struct sockaddr*)&saddr, sizeof(saddr));
    listen(server_fd, 5);
    pthread_mutex_init(&lock, NULL);
    while (1) {
        new_sock = accept(server_fd, (struct sockaddr*)&caddr, &clen);
        pthread_mutex_lock(&lock);
        int i;
        for (i=0;i<MAX_CLIENTS;i++)
            if (client_sockets[i]==0){client_sockets[i]=new_sock;pthread_create(&tid,NULL,client_handler,&new_sock);pthread_detach(tid);break;}
        if (i==MAX_CLIENTS){send(new_sock,"Server is full!\n",16,0);close(new_sock);}
        pthread_mutex_unlock(&lock);
    }
    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
