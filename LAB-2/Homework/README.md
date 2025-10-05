# LAB-2: TCP Echo Server with Timeout Mechanism

## 專案描述

這是一個基於TCP協定的Echo伺服器實作，具備超時機制和輸入驗證功能。客戶端可以發送兩個字串(A和B)以及學號給伺服器，伺服器會根據特定規則驗證輸入並回傳相應的結果。

## 學生資訊

- **學號**: D1210799
- **姓名**: 王建葦
- **課程**: 網路程式設計 (114-1)

## 檔案結構

```
LAB-2/
├── server.c          # TCP Echo 伺服器程式
├── client.c          # TCP Echo 客戶端程式
├── compile.sh        # 編譯腳本
└── README.md         # 本說明文件
```

## 功能需求

### 伺服器端功能
1. **TCP連線處理**: 監聽127.0.0.1:5678端口，接受客戶端連線
2. **輸入驗證**: 驗證字串A和B的合法性
   - 字串A: 長度必須在5-10個字元之間
   - 字串B: 長度必須為偶數
3. **超時機制**: 使用`select()`函數等待學號輸入，超時時間為5秒
4. **回應處理**: 根據不同情況回傳相應訊息

### 客戶端功能
1. **連線建立**: 連接到伺服器127.0.0.1:5678
2. **輸入處理**: 接收使用者輸入的字串A、B和學號
3. **輸入驗證**: 在發送前驗證字串A和B的合法性
4. **超時測試**: 支援不輸入學號來測試伺服器超時機制

## 程式流程

### 正常流程
1. 客戶端連接到伺服器
2. 客戶端輸入字串A（5-10字元）
3. 客戶端輸入字串B（偶數長度）
4. 客戶端輸入學號
5. 伺服器驗證A和B的合法性
6. 伺服器等待學號5秒
7. 伺服器回傳格式: `A B: [學號]`

### 錯誤處理
- **輸入不合法**: 伺服器回傳"error"
- **學號超時**: 伺服器回傳"Didn't receive student id"
- **空學號**: 伺服器回傳"Didn't receive student id"

## 編譯與執行

### 方法一：使用編譯腳本
```bash
chmod +x compile.sh
./compile.sh
```

### 方法二：手動編譯
```bash
# 編譯伺服器
gcc -o server server.c

# 編譯客戶端
gcc -o client client.c
```

### 執行步驟
1. **啟動伺服器**（在終端機1）:
   ```bash
   ./server
   ```

2. **啟動客戶端**（在終端機2）:
   ```bash
   ./client
   ```

## 使用範例

### 範例1：正常操作
```
客戶端輸出:
Connected to server 127.0.0.1:5678
Input A: HELLO
Input B: ALBERT
Input Student ID: D1210799
HELLO ALBERT: [D1210799]

伺服器輸出:
Server started on 127.0.0.1:5678 ....
Client connected: 127.0.0.1
Received A="HELLO" (5), B="ALBERT" (6)
```

### 範例2：輸入驗證失敗
```
客戶端輸出:
Input A: HI
Input B: WORLD
error

伺服器輸出:
Received A="HI" (2), B="WORLD" (5)
```

### 範例3：學號超時
```
客戶端輸出:
Input A: HELLO
Input B: WORLD
Input Student ID: [直接按Enter，不輸入任何內容]
Didn't receive student id

伺服器輸出:
Received A="HELLO" (5), B="WORLD" (5)
[等待5秒後]
```

## 詳細程式碼解析

### 伺服器端程式碼 (server.c) 詳細解析

#### 1. 標頭檔與常數定義
```c
#include <sys/socket.h>    // Socket相關函數
#include <netinet/in.h>   // 網路位址結構
#include <arpa/inet.h>    // 網路位址轉換函數
#include <unistd.h>       // 系統呼叫函數
#include <stdio.h>        // 標準輸入輸出
#include <string.h>       // 字串處理函數
#include <stdlib.h>       // 標準函式庫
#include <sys/select.h>   // Select函數

#define BUFFER_SIZE 256   // 緩衝區大小定義
```

#### 2. 變數宣告與初始化
```c
struct sockaddr_in server, client;  // 伺服器和客戶端位址結構
int sock, csock, readsize, addresssize;  // Socket描述符和相關變數
char A[BUFFER_SIZE], B[BUFFER_SIZE], ID[BUFFER_SIZE], sendbuf[BUFFER_SIZE];  // 資料緩衝區

// 初始化伺服器位址結構
bzero(&server, sizeof(server));           // 清空記憶體
server.sin_family = AF_INET;              // 設定為IPv4協定
server.sin_addr.s_addr = inet_addr("127.0.0.1");  // 設定IP位址為本地迴環
server.sin_port = htons(5678);            // 設定端口號5678，轉換為網路位元組順序
```

#### 3. Socket建立與綁定
```c
// 建立Socket
sock = socket(AF_INET, SOCK_STREAM, 0);  // AF_INET: IPv4, SOCK_STREAM: TCP
if (sock < 0) { 
    perror("socket creation failed"); 
    exit(1); 
}

// 綁定Socket到指定位址和端口
if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("bind failed");
    close(sock);
    exit(1);
}
```

#### 4. 監聽與接受連線
```c
listen(sock, 5);  // 開始監聽，最多5個等待連線
printf("Server started on 127.0.0.1:5678 ....\n");

// 接受客戶端連線
addresssize = sizeof(client);
csock = accept(sock, (struct sockaddr*)&client, &addresssize);
if (csock < 0) { 
    perror("accept failed"); 
    close(sock); 
    exit(1); 
}

printf("Client connected: %s\n", inet_ntoa(client.sin_addr));
```

#### 5. 主要處理迴圈 - 接收字串A和B
```c
while (1) {
    // 清空所有緩衝區，確保資料乾淨
    memset(A, 0, BUFFER_SIZE);
    memset(B, 0, BUFFER_SIZE);
    memset(ID, 0, BUFFER_SIZE);
    memset(sendbuf, 0, BUFFER_SIZE);

    // 接收字串A
    readsize = recv(csock, A, sizeof(A) - 1, 0);  // 接收資料
    if (readsize <= 0) break;  // 連線中斷或錯誤
    A[readsize] = '\0';        // 確保字串結尾
    int lenA = strlen(A);      // 計算字串長度

    // 接收字串B
    readsize = recv(csock, B, sizeof(B) - 1, 0);
    if (readsize <= 0) break;
    B[readsize] = '\0';
    int lenB = strlen(B);

    printf("Received A=\"%s\" (%d), B=\"%s\" (%d)\n", A, lenA, B, lenB);
```

#### 6. 輸入驗證邏輯
```c
    // 檢查字串A和B的合法性
    if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
        strcpy(sendbuf, "error");                    // 準備錯誤訊息
        send(csock, sendbuf, strlen(sendbuf), 0);   // 發送錯誤訊息
        continue;                                    // 跳過本次迴圈，等待下次輸入
    }
```

#### 7. 超時機制實作 - 關鍵技術
```c
    // 清空Socket緩衝區中的殘留資料
    fd_set readfds;
    struct timeval timeout;
    char tmpbuf[BUFFER_SIZE];
    while (recv(csock, tmpbuf, sizeof(tmpbuf), MSG_DONTWAIT) > 0);  // 非阻塞接收，清空殘留資料

    // 設定select()等待學號輸入，超時時間5秒
    FD_ZERO(&readfds);        // 清空檔案描述符集合
    FD_SET(csock, &readfds);  // 將客戶端Socket加入監聽集合
    timeout.tv_sec = 5;       // 設定超時時間為5秒
    timeout.tv_usec = 0;      // 微秒部分設為0

    // 使用select()等待資料到達或超時
    int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);
    
    if (ret == 0) {
        // select()回傳0表示超時，沒有收到任何資料
        strcpy(sendbuf, "Didn't receive student id");
        send(csock, sendbuf, strlen(sendbuf), 0);
        continue;  // 跳過本次迴圈
    }
```

#### 8. 接收學號並處理
```c
    // 接收學號
    readsize = recv(csock, ID, sizeof(ID) - 1, 0);
    if (readsize <= 0) break;
    ID[readsize] = '\0';

    // 檢查學號是否為空字串
    if (strlen(ID) == 0) {
        strcpy(sendbuf, "Didn't receive student id");
        send(csock, sendbuf, strlen(sendbuf), 0);
        continue;
    }

    // 格式化回應訊息並發送
    snprintf(sendbuf, sizeof(sendbuf), "%s %s: [%s]", A, B, ID);
    send(csock, sendbuf, strlen(sendbuf), 0);
}
```

#### 9. 資源清理
```c
printf("Client disconnected.\n");
close(csock);  // 關閉客戶端Socket
close(sock);   // 關閉伺服器Socket
return 0;
```

### 客戶端程式碼 (client.c) 詳細解析

#### 1. 變數宣告與伺服器位址設定
```c
struct sockaddr_in server;  // 伺服器位址結構
int sock;                   // Socket描述符
char A[BUFFER_SIZE], B[BUFFER_SIZE], ID[BUFFER_SIZE], recvbuf[BUFFER_SIZE];  // 資料緩衝區

// 設定伺服器位址
bzero(&server, sizeof(server));
server.sin_family = AF_INET;
server.sin_addr.s_addr = inet_addr("127.0.0.1");  // 連接到本地伺服器
server.sin_port = htons(5678);                     // 端口5678
```

#### 2. Socket建立與連線
```c
// 建立Socket
sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) { 
    perror("socket creation failed"); 
    exit(1); 
}

// 連接到伺服器
if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("connect failed");
    close(sock);
    exit(1);
}

printf("Connected to server 127.0.0.1:5678\n");
```

#### 3. 主要處理迴圈 - 使用者輸入處理
```c
while (1) {
    // 輸入字串A
    printf("Input A: ");
    if (fgets(A, sizeof(A), stdin) == NULL) break;  // 讀取使用者輸入
    int lenA = strlen(A);
    if (A[lenA - 1] == '\n') A[lenA - 1] = '\0';   // 移除換行符號
    lenA = strlen(A);                              // 重新計算長度

    // 輸入字串B
    printf("Input B: ");
    if (fgets(B, sizeof(B), stdin) == NULL) break;
    int lenB = strlen(B);
    if (B[lenB - 1] == '\n') B[lenB - 1] = '\0';
    lenB = strlen(B);
```

#### 4. 客戶端輸入驗證
```c
    // 在發送前先驗證輸入合法性
    if (lenA < 5 || lenA > 10 || lenB % 2 != 0) {
        printf("error\n");
        continue;  // 輸入不合法，重新開始
    }
```

#### 5. 資料傳輸策略
```c
    // 分別發送A和B，避免TCP合併
    send(sock, A, strlen(A), 0);     // 發送字串A
    usleep(300000);                  // 等待300毫秒，避免資料合併
    send(sock, B, strlen(B), 0);     // 發送字串B
    usleep(300000);                  // 再次等待
```

#### 6. 學號輸入與超時測試
```c
    // 輸入學號
    printf("Input Student ID: ");
    if (fgets(ID, sizeof(ID), stdin) == NULL) break;
    int lenID = strlen(ID);
    if (lenID > 0 && ID[lenID - 1] == '\n') ID[lenID - 1] = '\0';

    // 處理學號輸入
    if (strlen(ID) == 0) {
        // 使用者直接按Enter，不發送任何資料
        // 這會觸發伺服器的select()超時機制
        sleep(6);  // 等待6秒，確保伺服器超時
    } else {
        // 有輸入學號，發送給伺服器
        usleep(300000);
        send(sock, ID, strlen(ID), 0);
    }
```

#### 7. 接收伺服器回應
```c
    // 接收伺服器回應
    int readsize = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
    if (readsize <= 0) break;  // 連線中斷
    recvbuf[readsize] = '\0';  // 確保字串結尾
    printf("%s\n", recvbuf);   // 顯示伺服器回應
}
```

#### 8. 資源清理
```c
close(sock);  // 關閉Socket連線
return 0;
```

### 關鍵技術深度解析

#### 1. Select() 超時機制
```c
// 這是整個程式的核心技術
fd_set readfds;           // 檔案描述符集合
struct timeval timeout;   // 超時時間結構

FD_ZERO(&readfds);        // 清空集合
FD_SET(csock, &readfds); // 加入要監聽的Socket
timeout.tv_sec = 5;       // 5秒超時
timeout.tv_usec = 0;      // 0微秒

// select()會阻塞等待，直到：
// 1. 有資料可讀 (ret > 0)
// 2. 超時 (ret == 0)
// 3. 發生錯誤 (ret < 0)
int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);
```

#### 2. 資料清理技術
```c
// 使用非阻塞接收清空殘留資料
char tmpbuf[BUFFER_SIZE];
while (recv(csock, tmpbuf, sizeof(tmpbuf), MSG_DONTWAIT) > 0);
// MSG_DONTWAIT: 非阻塞模式，立即回傳
// 這個迴圈會持續讀取直到Socket緩衝區為空
```

#### 3. TCP資料分離技術
```c
// 客戶端使用延遲避免TCP合併
send(sock, A, strlen(A), 0);
usleep(300000);  // 300毫秒延遲
send(sock, B, strlen(B), 0);
usleep(300000);  // 再次延遲
```

#### 4. 記憶體管理
```c
// 每次迴圈開始時清空所有緩衝區
memset(A, 0, BUFFER_SIZE);
memset(B, 0, BUFFER_SIZE);
memset(ID, 0, BUFFER_SIZE);
memset(sendbuf, 0, BUFFER_SIZE);
```

### 程式執行流程圖

```
客戶端                    伺服器
  |                        |
  |-- connect() ---------->|-- accept()
  |                        |
  |<-- "Connected" --------|
  |                        |
  |-- send(A) ------------>|-- recv(A)
  |                        |
  |-- send(B) ------------>|-- recv(B)
  |                        |
  |                        |-- 驗證A,B合法性
  |                        |
  |                        |-- select()等待學號(5秒)
  |                        |
  |-- send(ID) ----------->|-- recv(ID) [如果select()成功]
  |                        |
  |<-- response -----------|-- send(response)
  |                        |
  |-- 顯示結果             |
```

### 錯誤處理機制

1. **Socket建立失敗**: 使用`perror()`顯示錯誤並退出
2. **連線失敗**: 關閉Socket並退出
3. **接收失敗**: 檢查`recv()`回傳值，小於等於0時退出迴圈
4. **輸入驗證**: 客戶端和伺服器都有驗證機制
5. **超時處理**: 使用`select()`實現優雅的超時處理

## 系統需求

- **作業系統**: Linux/Unix系統
- **編譯器**: GCC
- **函式庫**: 
  - `sys/socket.h` - Socket程式設計
  - `netinet/in.h` - 網路位址結構
  - `arpa/inet.h` - 網路位址轉換
  - `unistd.h` - 系統呼叫
  - `stdio.h` - 標準輸入輸出
  - `string.h` - 字串處理
  - `stdlib.h` - 標準函式庫
  - `sys/select.h` - Select函數