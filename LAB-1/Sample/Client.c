#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 使用 sockaddr_in 結構指定端點位址
    struct sockaddr_in server, client;
    int sock, csock, readsize, addresssize;
    char buf[256];

    bzero(&server, sizeof(server));   // 將 server 初始化（set 0）
    server.sin_family = PF_INET;      // 指定家族
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 127.0.0.1 代表本機 IP
    server.sin_port = htons(5678);    // 設定 port 號

    sock = socket(PF_INET, SOCK_STREAM, 0); // 設定傳輸協定，並回傳 descriptor
    bind(sock, (struct sockaddr*)&server, sizeof(server)); // 設定 local address
    listen(sock, 5);

    addresssize = sizeof(client);  // 指定 client 結構大小
    csock = accept(sock, (struct sockaddr*)&client, &addresssize);

    // 接收資料並回傳資料長度
    readsize = recv(csock, buf, sizeof(buf), 0);

    // 將資料長度大小加上 '\0' 變成字串
    buf[readsize] = '\0';
    printf("Read Message: %s\n", buf);

    close(sock);
}
