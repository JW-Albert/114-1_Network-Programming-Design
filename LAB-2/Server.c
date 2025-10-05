#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h> // for select()
#include <time.h>

#define True 1
#define False 0
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

    // 建立 TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // 綁定
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind failed");
        close(sock);
        exit(1);
    }

    listen(sock, 5);
    printf("Server started on 127.0.0.1:5678 ...\n");

    addresssize = sizeof(client);
    csock = accept(sock, (struct sockaddr*)&client, &addresssize);
    if (csock < 0) {
        perror("accept failed");
        close(sock);
        exit(1);
    }

    printf("Client connected: %s\n", inet_ntoa(client.sin_addr));

    while (True) {
        memset(A, 0, BUFFER_SIZE);
        memset(B, 0, BUFFER_SIZE);
        memset(ID, 0, BUFFER_SIZE);
        memset(sendbuf, 0, BUFFER_SIZE);

        // ---- Step 1: 接收 A ----
        readsize = recv(csock, A, sizeof(A) - 1, 0);
        if (readsize <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        A[readsize - 1] = '\0'; // 去除換行符
        int lenA = strlen(A);

        // ---- Step 2: 接收 B ----
        readsize = recv(csock, B, sizeof(B) - 1, 0);
        if (readsize <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        B[readsize - 1] = '\0';
        int lenB = strlen(B);

        printf("Received A=\"%s\" (%d), B=\"%s\" (%d)\n", A, lenA, B, lenB);

        // ---- Step 3: 驗證 A/B 是否合法 ----
        if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
            strcpy(sendbuf, "error");
            send(csock, sendbuf, strlen(sendbuf), 0);
            printf("Invalid input -> sent 'error'\n");
            continue;
        }

        // ---- Step 4: 使用 select() 等待 5 秒接收學號 ----
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(csock, &readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("select() error");
            break;
        } else if (ret == 0) {
            // timeout
            strcpy(sendbuf, "Didn't receive student id");
            send(csock, sendbuf, strlen(sendbuf), 0);
            printf("Timeout: didn't receive student id\n");
            continue;
        }

        // ---- Step 5: 讀取學號 ----
        readsize = recv(csock, ID, sizeof(ID) - 1, 0);
        if (readsize <= 0) {
            printf("Client disconnected during ID recv.\n");
            break;
        }
        ID[readsize - 1] = '\0';

        // ---- Step 6: 回傳結果 ----
        snprintf(sendbuf, sizeof(sendbuf), "%s %s: [%s]", A, B, ID);
        send(csock, sendbuf, strlen(sendbuf), 0);
        printf("Sent: %s\n", sendbuf);
    }

    close(csock);
    close(sock);
    return 0;
}
