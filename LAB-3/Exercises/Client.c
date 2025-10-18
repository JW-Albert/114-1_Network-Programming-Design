// ============================================
// Chat Client for LAB3 (pthread version)
// 可接收訊息與發送聊天文字
// 若伺服器滿員則顯示「Server is full!」
// ============================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;

void *receive_handler(void *arg) {
    char msg[BUFFER_SIZE];
    while (1) {
        memset(msg, 0, sizeof(msg));
        int len = recv(sock, msg, sizeof(msg) - 1, 0);
        if (len <= 0) break;
        msg[len] = '\0';
        printf("%s", msg);
        fflush(stdout);
    }
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in server_addr;
    pthread_t recv_thread;
    char msg[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // 嘗試接收伺服器滿員訊息
    char test[BUFFER_SIZE];
    recv(sock, test, sizeof(test) - 1, MSG_DONTWAIT);
    if (strstr(test, "Server is full!")) {
        printf("%s", test);
        close(sock);
        return 0;
    }

    printf("Connected to server.\nType messages or EXIT! to quit.\n\n");
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    while (1) {
        memset(msg, 0, sizeof(msg));
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = '\0';
        if (strcmp(msg, "EXIT!") == 0) {
            send(sock, msg, strlen(msg), 0);
            break;
        }
        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    return 0;
}
