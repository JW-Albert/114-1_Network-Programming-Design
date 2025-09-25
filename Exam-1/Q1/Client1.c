#include <sys/socket.h>   // 提供 socket(), connect(), send(), recv() 等 TCP/UDP 函式
#include <netinet/in.h>   // 提供 sockaddr_in 結構，用來設定 IPv4 位址與 port
#include <arpa/inet.h>    // 提供 inet_addr() (字串轉 IP)、htons() (轉換 port 成網路序)
#include <unistd.h>       // 提供 read(), write(), close() 等系統呼叫
#include <stdio.h>        // 提供標準輸入輸出 (printf, perror, fgets)
#include <string.h>       // 提供字串處理 (strlen, bzero)
#include <stdlib.h>       // 提供 exit() 系統呼叫
#include <ctype.h>        // 提供字元處理 (isalpha, toupper)

int main() {
    struct sockaddr_in server; // 用來存放 server 的位址資訊 (IP + port)
    int sock;                  // client socket 描述子
    char buf[256], recvbuf[512]; // buf: 使用者輸入的字串, recvbuf: 接收 server 回傳的結果

    // 初始化 server 結構，全部設為 0，避免殘留垃圾數值
    bzero( &server, sizeof(server) );

    server.sin_family = AF_INET;              // 指定使用 IPv4
    server.sin_addr.s_addr = inet_addr( "127.0.0.1" ); // 指定伺服器 IP
    server.sin_port = htons( 5678 );            // 指定伺服器 port (5678)，並轉成網路位元組序

    // 建立一個 TCP socket (SOCK_STREAM)，成功會回傳一個描述子 (整數)
    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock < 0 ) { // 如果小於 0，代表建立失敗
        perror( "socket creation failed" ); // 印出錯誤訊息
        exit( 1 ); // 強制結束程式
    }

    // 嘗試連線到 server (使用 server 結構資訊)
    if ( connect( sock, (struct sockaddr*)&server, sizeof(server) ) < 0 ) {
        perror( "connect failed" ); // 連線失敗
        exit( 1 );
    }

    // 進入迴圈，不斷從鍵盤讀取使用者輸入
    // fgets() 會讀一整行 (包含換行)，直到按下 CTRL+D (EOF) 才會結束
    while ( fgets( buf, sizeof(buf), stdin ) != NULL ) {
        // 把轉換後的字串送到 server
        send( sock, buf, strlen(buf), 0 );

        // 等待並接收 server 回傳的處理結果
        int readsize = recv( sock, recvbuf, sizeof(recvbuf)-1, 0 );
        recvbuf[readsize] = '\0'; // 在最後補上 '\0' 讓它成為合法 C 字串

        // 印出 server 回傳的結果，例如：
        printf( "%s\n", recvbuf );
    }

    // 如果 fgets() 回傳 NULL，代表遇到 EOF (例如使用者按下 CTRL+D)
    // 這時候要主動關閉 client 與 server 的連線
    close( sock );
}
