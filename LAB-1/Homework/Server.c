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
    server.sin_family = AF_INET;              // IPv4
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 本機 IP
    server.sin_port = htons(5678);            // 使用 port 5678

    // 建立 TCP socket
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
        // 從 client 接收資料
        readsize = recv(csock, buf, sizeof(buf)-1, 0);
        if (readsize <= 0) { // 如果 client 關閉連線
            printf("Client has closed the connection.\n");
            close(sock);
            exit(0);
        }

        buf[readsize] = '\0'; // 加上字串結尾符號

        // 統計字母與數字數量
        int letters = 0, numbers = 0;
        for (int i = 0; buf[i]; i++) {
            if (isalpha(buf[i])) letters++;
            else if (isdigit(buf[i])) numbers++;
        }

        // 處理字母：每個字母 +1
        char letters_part[128] = "";
        char numbers_part[128] = "";
        for (int i = 0; buf[i]; i++) {
            if (isalpha(buf[i])) {
                char c = buf[i] + 1;   // 英文字母往後移一位
                if (c > 'Z') c = 'A'; // 若超過 Z 則回到 A
                strncat(letters_part, &c, 1);
                strncat(letters_part, " ", 1);
            } else if (isdigit(buf[i])) {
                strncat(numbers_part, &buf[i], 1);
                strncat(numbers_part, " ", 1);
            }
        }

        // 組合回傳訊息
        char sendbuf[512];
        sprintf(sendbuf, "letters: %d numbers: %d [%s] and [%s]",
                letters, numbers, numbers_part, letters_part);

        // 傳送結果給 client
        send(csock, sendbuf, strlen(sendbuf), 0);
    }
}
