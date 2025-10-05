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

## 技術實作細節

### 伺服器端關鍵技術
- **Socket程式設計**: 使用`socket()`, `bind()`, `listen()`, `accept()`
- **非阻塞I/O**: 使用`select()`實現超時機制
- **資料清理**: 使用`MSG_DONTWAIT`清理殘留資料
- **記憶體管理**: 使用`memset()`確保buffer清空

### 客戶端關鍵技術
- **Socket連線**: 使用`connect()`建立TCP連線
- **輸入處理**: 使用`fgets()`安全讀取使用者輸入
- **資料傳輸**: 使用`send()`和`recv()`進行網路通訊
- **延遲控制**: 使用`usleep()`避免資料合併

### 超時機制實作
```c
// 伺服器端超時處理
fd_set readfds;
struct timeval timeout;
FD_ZERO(&readfds);
FD_SET(csock, &readfds);
timeout.tv_sec = 5;
timeout.tv_usec = 0;

int ret = select(csock + 1, &readfds, NULL, NULL, &timeout);
if (ret == 0) {
    // 超時處理
    strcpy(sendbuf, "Didn't receive student id");
    send(csock, sendbuf, strlen(sendbuf), 0);
}
```

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

## 注意事項

1. **端口使用**: 程式使用5678端口，請確保該端口未被其他程式佔用
2. **防火牆設定**: 如遇到連線問題，請檢查防火牆設定
3. **權限問題**: 在某些系統上可能需要管理員權限來綁定端口
4. **編譯警告**: 編譯時可能出現一些警告，但不影響程式正常運行

## 測試建議

1. **基本功能測試**: 測試正常的字串組合和學號輸入
2. **邊界值測試**: 測試字串A的長度邊界（5和10字元）
3. **奇偶數測試**: 測試字串B的奇數和偶數長度
4. **超時測試**: 測試不輸入學號的超時機制
5. **錯誤處理測試**: 測試各種無效輸入的處理

## 已知問題與限制

1. **單一客戶端**: 目前只支援單一客戶端連線
2. **字元編碼**: 僅支援ASCII字元
3. **錯誤恢復**: 某些錯誤情況下程式會直接退出
4. **資源清理**: 在異常情況下可能無法完全清理資源

## 未來改進方向

1. **多客戶端支援**: 使用多執行緒或非阻塞I/O支援多客戶端
2. **日誌記錄**: 加入詳細的日誌記錄功能
3. **配置檔案**: 支援透過配置檔案設定參數
4. **錯誤處理**: 改善錯誤處理和恢復機制
5. **效能優化**: 優化網路傳輸效能


---

**最後更新**: 2025年1月
**版本**: 1.0
