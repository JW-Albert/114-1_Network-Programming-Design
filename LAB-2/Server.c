#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int main(void) {
    struct sockaddr_in server, client;
    int sock, csock, readsize, addresssize;
    char A[BUFFER_SIZE], B[BUFFER_SIZE], ID[BUFFER_SIZE], sendbuf[BUFFER_SIZE];

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

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind failed");
        close(sock);
        exit(1);
    }

    listen(sock, 5);
    printf("Server started on 127.0.0.1:5678 ....\n");

    addresssize = sizeof(client);
    csock = accept(sock, (struct sockaddr*)&client, &addresssize);
    if (csock < 0) {
        perror("accept failed");
        close(sock);
        exit(1);
    }

    printf("Client connected: %s\n", inet_ntoa(client.sin_addr));

    while (1) {
        memset(A, 0, BUFFER_SIZE);
        memset(B, 0, BUFFER_SIZE);
        memset(ID, 0, BUFFER_SIZE);
        memset(sendbuf, 0, BUFFER_SIZE);

        // 收 A
        readsize = recv(csock, A, sizeof(A) - 1, 0);
        if (readsize <= 0) break;
        A[readsize] = '\0';
        int lenA = strlen(A);

        // 收 B
        readsize = recv(csock, B, sizeof(B) - 1, 0);
        if (readsize <= 0) break;
        B[readsize] = '\0';
        int lenB = strlen(B);

        printf("Received A=\"%s\" (%d), B=\"%s\" (%d)\n", A, lenA, B, lenB);

        // 檢查字串 A, B
        if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
            strcpy(sendbuf, "error");
            send(csock, sendbuf, strlen(sendbuf), 0);
            continue;
        }

        // 等待 5 秒學號
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(csock, &readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);
        if (ret == 0) {
            strcpy(sendbuf, "Didn't receive student id");
            send(csock, sendbuf, strlen(sendbuf), 0);
            continue;
        }

        // 收學號
        readsize = recv(csock, ID, sizeof(ID) - 1, 0);
        if (readsize <= 0) break;
        ID[readsize] = '\0';

        // 回傳字串
        snprintf(sendbuf, sizeof(sendbuf), "%s %s: [%s]", A, B, ID);
        send(csock, sendbuf, strlen(sendbuf), 0);
    }

    printf("Client disconnected.\n");
    close(csock);
    close(sock);
    return 0;
}
