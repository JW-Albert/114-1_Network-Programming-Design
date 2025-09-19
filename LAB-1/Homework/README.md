# TCP Socket Programming Homework 1

## 學生資訊
- **班級**: 資訊三乙
- **學號**: D1210799
- **姓名**: 王建葦
- **課程**: 網路程式設計

## 檔案內容
1. README.md 純文字說明文件
2. Demo.PNG 程式碼執行照片
3. Client.c 客戶端程式碼
4. Server.c 伺服端程式碼
5. compile.sh 編譯文件 (選擇性)

## 作業內容概述

本作業實作了一個基於 TCP Socket 的客戶端-伺服器通訊程式，主要功能如下：

1. **Client端 (Client.c)**: 
   - 連接到伺服器
   - 從鍵盤讀取使用者輸入
   - 將輸入字串中的字母轉換為大寫
   - 發送資料到伺服器並接收處理結果

2. **Server端 (Server.c)**:
   - 監聽指定port (5678)
   - 接受客戶端連線
   - 分析接收到的字串，統計字母和數字數量
   - 對字母進行凱撒密碼加密 (向後移一位)
   - 回傳處理結果給客戶端

## 程式編譯與執行

### 編譯指令
```bash
# 編譯伺服器程式
gcc -o server Server.c

# 編譯客戶端程式
gcc -o client Client.c
```

### 執行步驟
1. **啟動伺服器** (在終端機中執行):
   ```bash
   ./server
   ```
   伺服器會開始監聽port 5678，等待客戶端連線

2. **啟動客戶端** (在另一個終端機中執行):
   ```bash
   ./client
   ```
   客戶端會連接到伺服器，然後等待使用者輸入

3. **測試功能**:
   - 在客戶端終端機中輸入任意字串 (包含字母和數字)
   - 按 Enter 發送
   - 觀察伺服器回傳的處理結果
   - 按 Ctrl+D 結束程式

## 程式碼詳細說明

### Client.c 程式碼分析

#### 標頭檔引入
```c
#include <sys/socket.h>   // Socket相關函式
#include <netinet/in.h>   // IPv4位址結構
#include <arpa/inet.h>    // IP/Port轉換函式
#include <unistd.h>       // 系統呼叫
#include <stdio.h>        // 標準輸入輸出
#include <string.h>       // 字串處理
#include <stdlib.h>       // 程式控制
#include <ctype.h>        // 字元處理
```

#### 主要變數宣告
- `struct sockaddr_in server`: 儲存伺服器位址資訊
- `int sock`: 客戶端socket描述子
- `char buf[256]`: 使用者輸入緩衝區
- `char recvbuf[512]`: 接收伺服器回應的緩衝區

#### 核心功能流程

1. **Socket建立與連線**:
   ```c
   sock = socket(AF_INET, SOCK_STREAM, 0);  // 建立TCP socket
   connect(sock, (struct sockaddr*)&server, sizeof(server));  // 連線到伺服器
   ```

2. **資料處理迴圈**:
   - 使用`fgets()`讀取使用者輸入
   - 將字母轉換為大寫: `toupper(buf[i])`
   - 使用`send()`發送資料到伺服器
   - 使用`recv()`接收伺服器回應
   - 印出處理結果

### Server.c 程式碼分析

#### 主要變數宣告
- `struct sockaddr_in server, client`: 伺服器和客戶端位址結構
- `int sock`: 伺服器socket描述子
- `int csock`: 客戶端連線socket描述子
- `char buf[256]`: 接收客戶端資料的緩衝區

#### 核心功能流程

1. **Socket建立與綁定**:
   ```c
   sock = socket(AF_INET, SOCK_STREAM, 0);  // 建立TCP socket
   bind(sock, (struct sockaddr*)&server, sizeof(server));  // 綁定到指定port
   listen(sock, 5);  // 開始監聽，最多5個連線排隊
   ```

2. **接受客戶端連線**:
   ```c
   csock = accept(sock, (struct sockaddr*)&client, &addresssize);
   ```

3. **資料處理迴圈**:
   - 使用`recv()`接收客戶端資料
   - 統計字母和數字數量
   - 對字母進行凱撒密碼加密 (A→B, B→C, ..., Z→A)
   - 使用`send()`回傳處理結果

#### 字元處理邏輯
```c
// 統計字母和數字
if (isalpha(ch)) letters++;
else if (isdigit(ch)) numbers++;

// 凱撒密碼加密
char c = toupper(ch) + 1;
if (c > 'Z') c = 'A';  // 環狀處理
```

## 網路通訊協定

### TCP連線建立過程
1. **三次握手**: Client發起連線 → Server回應 → Client確認
2. **資料傳輸**: 使用可靠的TCP協定確保資料完整性
3. **連線關閉**: Client發送EOF或關閉socket

### 通訊格式
- **Client → Server**: 原始字串 (字母已轉大寫)
- **Server → Client**: `"letters: X numbers: Y [數字列表] and [加密字母列表]"`

## 執行結果

請參考 `Demo.PNG` 檔案查看實際執行結果的截圖。

### 範例執行流程
1. 輸入: `"Hello123World"`
2. Client處理: `"HELLO123WORLD"`
3. Server回應: `"letters: 10 numbers: 3 [1 2 3] and [I F M M P P X P S M E]"`

## 技術特點

### 安全性考量
- 使用 `bzero()` 初始化結構避免記憶體洩漏
- 檢查所有系統呼叫的返回值
- 適當的錯誤處理機制

### 效能優化
- 使用適當的緩衝區大小
- 避免不必要的字串複製
- 高效的凱撒密碼實作

### 程式設計模式
- 模組化設計，Client 和 Server 職責分離
- 清晰的錯誤處理流程
- 詳細的中文註解說明

## 注意事項

1. **Port衝突**: 如果 port 5678 被占用，請修改程式中的 port 號碼
2. **防火牆設定**: 確保系統防火牆允許該 port 的連線
3. **編譯環境**: 需要支援 POSIX 的 C 編譯器(如 gcc)
4. **執行順序**: 必須先啟動 Server 再啟動 Client

---

**開發環境**: Debian 13
**編譯器**: GCC
**協定**: TCP/IP
**Port**: 5678