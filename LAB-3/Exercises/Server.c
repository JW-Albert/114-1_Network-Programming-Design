// ============================================
// Multi-threaded Chat Server (LAB3 version)
// 限制僅兩人連線，第三人顯示「Server is full!」
// 並顯示每位 Client 的連線順序
// ============================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
pthread_mutex_t lock;

// 廣播訊息給所有連線中的 client（排除自己）
void broadcast_message(char *msg, int sender_sock) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_sock) {
            send(client_sockets[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

// 每個 client 的執行緒函式
void *handle_client(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int index = -1;

    // 找出自己的編號
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == sock) {
            index = i;
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    // 告訴 client 自己的連線順序
    char msg[50];
    sprintf(msg, "You are Client #%d\n", index + 1);
    send(sock, msg, strlen(msg), 0);

    sprintf(msg, "Client #%d joined the chat.\n", index + 1);
    broadcast_message(msg, sock);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break;

        buffer[len] = '\0';
        if (strcmp(buffer, "EXIT!") == 0) break;

        char send_buf[BUFFER_SIZE + 50];
        sprintf(send_buf, "Client #%d: %s", index + 1, buffer);
        broadcast_message(send_buf, sock);
    }

    // 離線處理
    close(sock);
    pthread_mutex_lock(&lock);
    client_sockets[index] = 0;
    pthread_mutex_unlock(&lock);

    sprintf(msg, "Client #%d left the chat.\n", index + 1);
    broadcast_message(msg, sock);

    pthread_exit(NULL);
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
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
    printf("Server started at 127.0.0.1:%d\n", PORT);

    while (1) {
        new_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (new_sock < 0) continue;

        pthread_mutex_lock(&lock);
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_sock;
                pthread_create(&tid, NULL, handle_client, &new_sock);
                pthread_detach(tid);
                printf("Client #%d connected.\n", i + 1);
                break;
            }
        }

        // 當超過兩人連線
        if (i == MAX_CLIENTS) {
            char full_msg[] = "Server is full!\n";
            send(new_sock, full_msg, strlen(full_msg), 0);
            close(new_sock);
            printf("A client was rejected (server full).\n");
        }
        pthread_mutex_unlock(&lock);
    }

    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
