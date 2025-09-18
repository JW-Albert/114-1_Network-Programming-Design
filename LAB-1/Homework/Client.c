#include <sys/socket.h>   // socket(), connect()
#include <netinet/in.h>   // sockaddr_in 結構
#include <arpa/inet.h>    // inet_addr(), htons()
#include <unistd.h>       // read(), write(), close()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>        // toupper()

int main() {
    struct sockaddr_in server;
    int sock;
    char buf[256], recvbuf[512];

    // 初始化 server 結構
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;              // IPv4
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 本機 IP
    server.sin_port = htons(5678);            // 使用 port 5678

    // 建立 TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // 連線到 server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        exit(1);
    }

    // 持續讀取使用者輸入直到 EOF (CTRL+D)
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        // 把所有字母轉成大寫
        for (int i = 0; buf[i]; i++) {
            if (isalpha(buf[i])) buf[i] = toupper(buf[i]);
        }

        // 傳送到 server
        send(sock, buf, strlen(buf), 0);

        // 接收 server 的回傳
        int readsize = recv(sock, recvbuf, sizeof(recvbuf)-1, 0);
        recvbuf[readsize] = '\0';

        // 印出結果
        printf("%s\n", recvbuf);
    }

    // EOF -> 關閉連線
    close(sock);
}
