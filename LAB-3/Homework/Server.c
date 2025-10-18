// =======================================================
// Multi-threaded TCP Chat Server (Homework 3)
// Using pthread — supports 3 Rooms and max 5 clients
// Host: 127.0.0.1, Port: 8080
// =======================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
char client_rooms[MAX_CLIENTS][10];
pthread_mutex_t lock;

// broadcast to all clients in the same room (except sender)
void broadcast_message(const char *msg, int sender_index, const char *room) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 &&
            strcmp(client_rooms[i], room) == 0 &&
            i != sender_index) {
            send(client_sockets[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

// broadcast to everyone (server announcement)
void broadcast_server(const char *msg) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            send(client_sockets[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

void *client_handler(void *arg) {
    int sock = *(int*)arg;
    int index = -1;
    char buffer[BUFFER_SIZE];
    char room[10] = "RoomA";

    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == sock) index = i;
    }
    strcpy(client_rooms[index], room);
    pthread_mutex_unlock(&lock);

    sprintf(buffer, "[Server]: A new client joined %s.\n", room);
    broadcast_message(buffer, index, room);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break; // client disconnected

        buffer[len] = '\0';

        // Handle EXIT!
        if (strcmp(buffer, "EXIT!") == 0) {
            sprintf(buffer, "[Server]: A client left %s.\n", client_rooms[index]);
            broadcast_message(buffer, index, client_rooms[index]);
            break;
        }

        // Change room command: enter A/B/C
        if (strncmp(buffer, "enter ", 6) == 0) {
            char newRoom[10];
            char roomLetter = buffer[6];
            if (roomLetter == 'A') strcpy(newRoom, "RoomA");
            else if (roomLetter == 'B') strcpy(newRoom, "RoomB");
            else if (roomLetter == 'C') strcpy(newRoom, "RoomC");
            else {
                send(sock, "Invalid room! Use A, B, or C.\n", 30, 0);
                continue;
            }

            pthread_mutex_lock(&lock);
            strcpy(client_rooms[index], newRoom);
            pthread_mutex_unlock(&lock);

            sprintf(buffer, "[Server]: You entered %s.\n", newRoom);
            send(sock, buffer, strlen(buffer), 0);

            sprintf(buffer, "[Server]: A client joined %s.\n", newRoom);
            broadcast_message(buffer, index, newRoom);
            continue;
        }

        // Normal chat message
        char msg[BUFFER_SIZE + 50];
        sprintf(msg, "[%s] Client%d: %s\n", client_rooms[index], index + 1, buffer);
        broadcast_message(msg, index, client_rooms[index]);
    }

    close(sock);
    pthread_mutex_lock(&lock);
    client_sockets[index] = 0;
    pthread_mutex_unlock(&lock);

    printf("Client %d disconnected.\n", index + 1);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    printf("Server starting...\nHost: 127.0.0.1\nPort: %d\n\n", PORT);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    pthread_mutex_init(&lock, NULL);
    printf("Server is running. Waiting for clients...\n");

    while (1) {
        new_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (new_sock < 0) continue;

        pthread_mutex_lock(&lock);
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_sock;
                pthread_create(&tid, NULL, client_handler, &new_sock);
                pthread_detach(tid);
                printf("Client %d connected.\n", i + 1);
                break;
            }
        }

        // Server full
        if (i == MAX_CLIENTS) {
            char msg[] = "Server is full!\n";
            send(new_sock, msg, strlen(msg), 0);
            close(new_sock);
        }
        pthread_mutex_unlock(&lock);
    }

    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
