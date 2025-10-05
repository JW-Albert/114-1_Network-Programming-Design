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
    int sock, csock, addresssize, readsize;
    char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE];

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5678);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket creation failed"); exit(1); }

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind failed");
        close(sock);
        exit(1);
    }

    listen(sock, 5);
    printf("Server started on 127.0.0.1:5678 ....\n");

    addresssize = sizeof(client);
    csock = accept(sock, (struct sockaddr*)&client, &addresssize);
    if (csock < 0) { perror("accept failed"); close(sock); exit(1); }

    printf("Client connected: %s\n", inet_ntoa(client.sin_addr));

    while (1) {
        memset(buf1, 0, BUFFER_SIZE);
        memset(buf2, 0, BUFFER_SIZE);
        memset(sendbuf, 0, BUFFER_SIZE);

        // 接收第一個數字
        readsize = recv(csock, buf1, sizeof(buf1) - 1, 0);
        if (readsize <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        buf1[readsize] = '\0';

        // 嘗試等待第二個數字（3 秒）
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(csock, &readfds);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);

        if (ret > 0) {
            // 收到第二個數字
            readsize = recv(csock, buf2, sizeof(buf2) - 1, 0);
            if (readsize > 0) {
                buf2[readsize] = '\0';
                int a = atoi(buf1);
                int b = atoi(buf2);
                int result = a * b;
                snprintf(sendbuf, sizeof(sendbuf), "Result: %d * %d = %d", a, b, result);
            } else {
                int a = atoi(buf1);
                int result = a * 100;
                snprintf(sendbuf, sizeof(sendbuf), "Timeout, %d * 100 = %d", a, result);
            }
        } else {
            // timeout 沒有第二個數字
            int a = atoi(buf1);
            int result = a * 100;
            snprintf(sendbuf, sizeof(sendbuf), "Timeout, %d * 100 = %d", a, result);
        }

        send(csock, sendbuf, strlen(sendbuf), 0);
    }

    close(csock);
    close(sock);
    return 0;
}
