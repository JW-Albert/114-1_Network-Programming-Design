#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main(void) {
    struct sockaddr_in server;  // server 位址結構
    int sock;
    char A[BUFFER_SIZE], B[BUFFER_SIZE], ID[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];

    // 初始化 server 結構
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // 固定 IP
    server.sin_port = htons(5678);                    // 固定 PORT

    // 建立 TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // 嘗試連線到 server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server 127.0.0.1:5678\n");
    printf("Enter A, B, and student ID (Ctrl+D to quit)\n\n");

    // 主迴圈
    while (1) {
        // ---- 輸入 A ----
        printf("Input A: ");
        if (fgets(A, sizeof(A), stdin) == NULL) break;
        int lenA = strlen(A);
        if (A[lenA - 1] == '\n') A[lenA - 1] = '\0';
        lenA = strlen(A);

        // ---- 輸入 B ----
        printf("Input B: ");
        if (fgets(B, sizeof(B), stdin) == NULL) break;
        int lenB = strlen(B);
        if (B[lenB - 1] == '\n') B[lenB - 1] = '\0';
        lenB = strlen(B);

        // ---- 檢查合法性 ----
        if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
            printf("error\n\n");
            continue;
        }

        // ---- 傳送 A、B ----
        send(sock, A, strlen(A) + 1, 0);
        send(sock, B, strlen(B) + 1, 0);

        // ---- 輸入學號 ----
        printf("Input Student ID (server waits 5 seconds): ");
        if (fgets(ID, sizeof(ID), stdin) == NULL) break;
        int lenID = strlen(ID);
        if (ID[lenID - 1] == '\n') ID[lenID - 1] = '\0';

        // 傳送學號
        send(sock, ID, strlen(ID) + 1, 0);

        // ---- 接收伺服器回覆 ----
        int readsize = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (readsize <= 0) {
            printf("Server closed connection.\n");
            break;
        }
        recvbuf[readsize] = '\0';
        printf("Server reply: %s\n\n", recvbuf);
    }

    printf("\nClient exiting...\n");
    close(sock);
    return 0;
}
