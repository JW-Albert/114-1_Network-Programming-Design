// =======================================================
// TCP Chat Client (Homework 3)
// Using pthread — supports joining/changing rooms
// Host: 127.0.0.1, Port: 8080
// =======================================================
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

    printf("Connecting to server...\nHost: 127.0.0.1\nPort: %d\n\n", PORT);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to chat server.\nCommands:\n");
    printf("  enter A/B/C  -> Join RoomA/B/C\n");
    printf("  EXIT!        -> Leave chat\n\n");

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

    printf("Disconnected.\n");
    return 0;
}
