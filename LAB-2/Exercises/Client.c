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
    struct sockaddr_in server;
    int sock;
    char num1[BUFFER_SIZE], num2[BUFFER_SIZE], recvbuf[BUFFER_SIZE];

    // 建立連線
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5678);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket creation failed"); exit(1); }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server 127.0.0.1:5678\n");

    while (1) {
        memset(num1, 0, BUFFER_SIZE);
        memset(num2, 0, BUFFER_SIZE);

        printf("Input first number: ");
        if (fgets(num1, sizeof(num1), stdin) == NULL) break;
        num1[strcspn(num1, "\n")] = '\0';  // 去掉換行
        if (strlen(num1) == 0) continue;

        send(sock, num1, strlen(num1), 0);

        printf("(You have 3 seconds to enter second number...)\n");

        // 等待 3 秒看看是否要輸入第二個數字
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds); // 監聽標準輸入
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int ret = select(1, &readfds, NULL, NULL, &timeout);
        if (ret > 0) {
            printf("Input second number: ");
            if (fgets(num2, sizeof(num2), stdin) != NULL) {
                num2[strcspn(num2, "\n")] = '\0';
                send(sock, num2, strlen(num2), 0);
            }
        } else {
            printf("No second number entered, will send only first.\n");
        }

        // 接收伺服器回傳結果
        int readsize = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (readsize <= 0) {
            printf("Server closed connection.\n");
            break;
        }
        recvbuf[readsize] = '\0';
        printf("%s\n\n", recvbuf);
    }

    close(sock);
    return 0;
}
