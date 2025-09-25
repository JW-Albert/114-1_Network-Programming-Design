#include <sys/socket.h>   // 提供 socket()、bind()、listen()、accept() 等 TCP/UDP 函式
#include <netinet/in.h>   // 提供 sockaddr_in 結構，定義 IPv4 網路位址
#include <arpa/inet.h>    // 提供 inet_addr()、htons() 等轉換 IP/port 的函式
#include <unistd.h>       // 提供 read()、write()、close() 等系統呼叫
#include <stdio.h>        // 標準輸入輸出函式庫 (printf, perror)
#include <string.h>       // 字串處理 (bzero, strlen, snprintf)
#include <stdlib.h>       // exit() 等系統控制
#include <ctype.h>        // 提供 isalpha()、isdigit()、toupper() 等字元判斷/轉換函式

#define True 1
#define False 0

int main( void ) {
    struct sockaddr_in server, client; // 用來儲存 Server 與 Client 的網路位址資訊 (IP、port)
    int sock, csock, readsize, addresssize; // sock: server socket，csock: client socket
    char buf[256]; // 暫存 client 傳來的資料

    // 初始化 server 結構，把內容全部清 0
    bzero(&server, sizeof(server));

    // 指定使用 IPv4
    server.sin_family = AF_INET;

    // 指定伺服器 IP
    server.sin_addr.s_addr = inet_addr( "127.0.0.1" );

    // 指定伺服器使用的 port，並用 htons() 轉換成網路位元組序
    server.sin_port = htons( 5678 );

    // 建立一個 TCP socket (SOCK_STREAM)，回傳 socket 描述子
    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock < 0 ) { // 如果小於 0 代表建立失敗
        perror( "socket creation failed" ); // 印出錯誤原因
        exit( 1 ); // 結束程式
    }

    // 將 socket 綁定到指定的 IP 與 port
    if ( bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
        perror( "bind failed" ); // 綁定失敗
        exit( 1 );
    }

    // 開始監聽，允許最多 5 個 client 排隊等待連線
    listen( sock, 5 );

    // 等待 client 連線，addresssize 要放進去 client 結構大小
    addresssize = sizeof( client );
    csock = accept( sock, (struct sockaddr*)&client, &addresssize );
    if ( csock < 0 ) { // 如果回傳小於 0，代表連線建立失敗
        perror( "accept failed" );
        exit( 1 );
    }

    // 進入無限迴圈，持續接收 client 資料
    while ( True ) {
        // 從 client socket 收資料，最多讀取 buf 大小-1 (留位置給 '\0')
        readsize = recv( csock, buf, sizeof(buf)-1, 0 );

        // 如果 readsize <= 0，代表 client 關閉連線或錯誤
        if ( readsize <= 0 ) {
            printf( "Client has closed the connection.\n" );
            close( sock ); // 關閉伺服器 socket
            exit( 0 );     // 結束程式
        }

        // 在最後補上 '\0' 讓 buf 變成合法 C 字串
        buf[readsize] = '\0';

        printf ( "%s\n", buf );

    
        for ( int i = 0; buf[i]; i++ ) {
            buf[i] ^= 'Z';
        } 
    
        // 建立要回傳給 client 的結果訊息
        char sendbuf[512];
        snprintf(sendbuf, sizeof(sendbuf), "%s", buf);
        printf ( "%s\n", buf );
    
        // 把結果送回給 client
        send( csock, sendbuf, strlen(sendbuf), 0 );
    }
}
