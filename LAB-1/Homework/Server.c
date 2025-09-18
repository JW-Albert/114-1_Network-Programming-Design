#include <sys/socket.h>   // socket(), bind(), listen(), accept()
#include <netinet/in.h>   // sockaddr_in 結構
#include <arpa/inet.h>    // inet_addr(), htons()
#include <unistd.h>       // read(), write(), close()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>        // isalpha(), isdigit()

int main() {
    struct sockaddr_in server, client; // 儲存 server 與 client 的位址資訊
    int sock, csock, readsize, addresssize;
    char buf[256];

    // 初始化 server 結構
    bzero(&server, sizeof(server));
    // IPv4
    server.sin_family = AF_INET;
    // 伺服器 IP
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 使用 port 5678
    server.sin_port = htons(5678);

    // 建立 TCP socket ， 如果創建失敗就離開程式
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // 綁定 socket 到本機位址與 port
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // 開始監聽，最多允許 5 個 client 排隊
    listen(sock, 5);

    // 等待 client 連線
    addresssize = sizeof(client);
    csock = accept(sock, (struct sockaddr*)&client, &addresssize);
    if (csock < 0) {
        perror("accept failed");
        exit(1);
    }

    while (1) {
        readsize = recv(csock, buf, sizeof(buf)-1, 0);
        if (readsize <= 0) {
            printf("Client has closed the connection.\n");
            close(sock);
            exit(0);
        }

        buf[readsize] = '\0';
    
        int letters = 0, numbers = 0;
        for (int i = 0; buf[i]; i++) {
            if (isalpha((unsigned char)buf[i])) letters++;
            else if (isdigit((unsigned char)buf[i])) numbers++;
        }
    
        char letters_part[128] = "";
        char numbers_part[128] = "";
    
        for (int i = 0; buf[i]; i++) {
            unsigned char ch = (unsigned char)buf[i];
    
            if (isalpha(ch)) {
                char c = toupper(ch) + 1;   // 保險：先轉大寫再 +1
                if (c > 'Z') c = 'A';       // Z 之後環回 A
                size_t off = strlen(letters_part);
                snprintf(letters_part + off, sizeof(letters_part) - off, "%c ", c);
            } else if (isdigit(ch)) {
                size_t off = strlen(numbers_part);
                snprintf(numbers_part + off, sizeof(numbers_part) - off, "%c ", ch);
            }
        }
    
        char sendbuf[512];
        snprintf(sendbuf, sizeof(sendbuf),
                 "letters: %d numbers: %d [%s] and [%s]",
                 letters, numbers, numbers_part, letters_part);
    
        send(csock, sendbuf, strlen(sendbuf), 0);
    }

}
