#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int main(void) {
    struct sockaddr_in server;
    int sock;
    char A[BUFFER_SIZE], B[BUFFER_SIZE], ID[BUFFER_SIZE], recvbuf[BUFFER_SIZE];

    // 初始化
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5678);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server 127.0.0.1:5678\n");

    while (1) {
        printf("Input A: ");
        if (fgets(A, sizeof(A), stdin) == NULL) break;
        int lenA = strlen(A);
        if (A[lenA - 1] == '\n') A[lenA - 1] = '\0';
        lenA = strlen(A);

        printf("Input B: ");
        if (fgets(B, sizeof(B), stdin) == NULL) break;
        int lenB = strlen(B);
        if (B[lenB - 1] == '\n') B[lenB - 1] = '\0';
        lenB = strlen(B);

        // 檢查字串合法性
        if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
            printf("error\n");
            continue;
        }

        // 傳送 A, B
        send(sock, A, strlen(A), 0);
        send(sock, B, strlen(B), 0);

        // 等 0.5 秒以確保 server select() 準備完成
        usleep(500000);

        printf("Input Student ID: ");
        if (fgets(ID, sizeof(ID), stdin) == NULL) break;
        int lenID = strlen(ID);
        if (ID[lenID - 1] == '\n') ID[lenID - 1] = '\0';

        send(sock, ID, strlen(ID), 0);

        int readsize = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (readsize <= 0) break;
        recvbuf[readsize] = '\0';
        printf("%s\n", recvbuf);
    }

    close(sock);
    return 0;
}
