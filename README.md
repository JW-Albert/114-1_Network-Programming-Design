# 網路程式設計課程紀錄 / Network Programming Design Course Record

## 課程資訊 / Course Information
- **學校 / University**: 逢甲大學 Feng Chia University
- **課程名稱 / Course Name**: 網路程式設計 (Network Programming Design)
- **學期 / Semester**: 114-1
- **學生 / Student**: 王建葦 (D1210799)
- **班級 / Class**: 資訊三乙 (IECS 3B)
- **授課教師 / Instructor**: 陳烈武

## Repository 簡介 / Repository Introduction

本 repository 記錄了網路程式設計課程的所有實驗作業、程式碼實作與學習筆記。內容包含 TCP/UDP Socket 程式設計、多執行緒網路程式、Java Socket 程式設計、以及相關的網路程式設計技術。

This repository documents all laboratory assignments, code implementations, and learning notes for the Network Programming Design course. It includes TCP/UDP Socket programming, multi-threaded network programming, Java Socket programming, and related network programming technologies.

## 目錄結構 / Directory Structure

```
114-1_Network-Programming-Design/
├── README.md                           # 本文件 / This file
├── .gitignore                          # Git 忽略檔案 / Git ignore file
│
├── LAB-1/                              # 實驗一：TCP Socket Programming
│   ├── Homework/                      # 作業檔案 / Assignment Files
│   │   ├── README.md                   # 作業說明文件（中文）/ Assignment Documentation (Chinese)
│   │   ├── README_EN.md                # 作業說明文件（英文）/ Assignment Documentation (English)
│   │   ├── Client.c                    # 客戶端程式碼 / Client Code
│   │   ├── Server.c                    # 伺服器程式碼 / Server Code
│   │   ├── compile.sh                  # 編譯腳本 / Compilation Script
│   │   └── Demo.PNG                    # 執行結果截圖 / Execution Screenshot
│   ├── Sample/                         # 範例程式碼 / Sample Code
│   │   ├── Client.c                    # 範例客戶端 / Sample Client
│   │   ├── Server.c                    # 範例伺服器 / Sample Server
│   │   └── compile.sh                  # 編譯腳本 / Compilation Script
│   └── LAB1-TCP Socket Programming in Linux.pptx  # 實驗講義 / Lab Materials
│
├── LAB-2/                              # 實驗二：TCP Echo Server with Timeout
│   ├── Homework/                       # 作業檔案 / Assignment Files
│   │   ├── README.md                   # 作業說明文件 / Assignment Documentation
│   │   ├── Server.c                    # TCP Echo 伺服器程式
│   │   ├── Client.c                    # TCP Echo 客戶端程式
│   │   ├── compile.sh                  # 編譯腳本 / Compilation Script
│   │   ├── DEMO-1.PNG                  # 執行結果截圖 1 / Execution Screenshot 1
│   │   ├── DEMO-2.PNG                  # 執行結果截圖 2 / Execution Screenshot 2
│   │   ├── server                      # 編譯後的伺服器執行檔 / Compiled Server Executable
│   │   ├── client                      # 編譯後的客戶端執行檔 / Compiled Client Executable
│   │   └── HW2.pdf                     # 作業說明 PDF / Assignment PDF
│   └── Exercises/                      # 練習題 / Exercises
│       ├── Server.c                    # 練習用伺服器程式
│       ├── Client.c                    # 練習用客戶端程式
│       ├── compile.sh                  # 編譯腳本
│       ├── DEMO.PNG                    # 執行結果截圖
│       ├── server                      # 編譯後的伺服器執行檔
│       └── client                      # 編譯後的客戶端執行檔
│
├── LAB-3/                              # 實驗三：Multi-threaded TCP Chatroom
│   ├── Homework/                       # 作業檔案 / Assignment Files
│   │   ├── README.md                   # 作業說明文件 / Assignment Documentation
│   │   ├── Server.c                    # 多執行緒聊天室伺服器程式
│   │   ├── Client.c                    # 多執行緒聊天室客戶端程式
│   │   ├── Compile.sh                  # 編譯腳本 / Compilation Script
│   │   ├── DEMO.PNG                    # 執行結果截圖 / Execution Screenshot
│   │   ├── server                      # 編譯後的伺服器執行檔 / Compiled Server Executable
│   │   └── client                      # 編譯後的客戶端執行檔 / Compiled Client Executable
│   └── Exercises/                      # 練習題 / Exercises
│       ├── Server.c                    # 練習用伺服器程式
│       ├── Client.c                    # 練習用客戶端程式
│       ├── Compile.sh                  # 編譯腳本
│       ├── DEMO.PNG                    # 執行結果截圖
│       ├── server                      # 編譯後的伺服器執行檔
│       └── client                      # 編譯後的客戶端執行檔
│
├── LAB-4/                              # 實驗四：Multi-Group Dynamic Chatroom (Java)
│   ├── Homework/                       # 作業檔案 / Assignment Files
│   │   ├── README.md                   # 作業說明文件（含完整程式碼講解）/ Assignment Documentation (with code explanation)
│   │   ├── src/                        # 原始碼目錄 / Source Code Directory
│   │   │   ├── Server.java             # 多群組動態聊天室伺服器（Java）
│   │   │   └── Client.java             # 多群組動態聊天室客戶端（Java）
│   │   ├── Demo/                       # 功能測試截圖 / Feature Test Screenshots
│   │   │   ├── Demo_01.Connect_*.PNG    # 連線測試截圖
│   │   │   ├── Demo_02.LEAVE_*.PNG     # 離開群組測試截圖
│   │   │   ├── Demo_03.CHANGE_*.PNG    # 群組切換測試截圖
│   │   │   ├── Demo_04.EXIT_*.PNG      # 伺服器關閉測試截圖
│   │   │   ├── Demo_05.HISTORY.PNG    # 歷史記錄功能測試截圖
│   │   │   └── Demo_06.ServerBroadcast_*.PNG  # 伺服器廣播測試截圖
│   │   ├── Compile.sh                  # 編譯腳本 / Compilation Script
│   │   ├── server.sh                   # 伺服器啟動腳本 / Server Startup Script
│   │   └── client.sh                   # 客戶端啟動腳本 / Client Startup Script
│   └── Exercises/                      # 練習題 / Exercises
│       ├── Server.java                 # 練習用伺服器程式（Java）
│       ├── Client.java                 # 練習用客戶端程式（Java）
│       └── Compile.sh                  # 編譯腳本
│
├── Exam-1/                             # 第一次上機考試 / First In-Class Exam
│   ├── LAB1-上機考_2025.pptx           # 考試講義 / Exam Materials
│   ├── Q1/                             # 題目一 / Question 1
│   │   ├── Server1.c                  # 伺服器程式
│   │   ├── Client1.c                  # 客戶端程式
│   │   ├── Server1.png                # 伺服器執行結果截圖
│   │   └── Client1.png                # 客戶端執行結果截圖
│   ├── Q2/                             # 題目二 / Question 2
│   │   ├── Server2.c                  # 伺服器程式
│   │   ├── Client2.c                  # 客戶端程式
│   │   ├── Server2.png                # 伺服器執行結果截圖
│   │   └── Client2.png                # 客戶端執行結果截圖
│   └── Q3(未完成)/                     # 題目三（未完成）/ Question 3 (Incomplete)
│
├── Exam-2/                             # 第二次上機考試 / Second In-Class Exam
│   ├── Q1/                             # 題目一 / Question 1
│   │   ├── Server1.c                  # 伺服器程式
│   │   ├── Client1.c                  # 客戶端程式
│   │   ├── compile.sh                 # 編譯腳本
│   │   ├── server                     # 編譯後的伺服器執行檔
│   │   └── client                     # 編譯後的客戶端執行檔
│   ├── Q1V2/                           # 題目一版本二 / Question 1 Version 2
│   ├── Q2/                             # 題目二 / Question 2
│   └── Q3/                             # 題目三 / Question 3
│
└── Exam-3/                             # 第三次上機考試 / Third In-Class Exam
    ├── Server1.c                       # 伺服器程式 1
    ├── Server2.c                       # 伺服器程式 2
    ├── Server3.c                       # 伺服器程式 3
    ├── Client1.c                       # 客戶端程式 1
    ├── Client2.c                       # 客戶端程式 2
    ├── Client3.c                       # 客戶端程式 3
    ├── server1                         # 編譯後的伺服器執行檔 1
    ├── server2                         # 編譯後的伺服器執行檔 2
    ├── server3                         # 編譯後的伺服器執行檔 3
    ├── client1                         # 編譯後的客戶端執行檔 1
    ├── client2                         # 編譯後的客戶端執行檔 2
    └── client3                         # 編譯後的客戶端執行檔 3
```

## 實驗內容 / Lab Contents

### LAB-1: TCP Socket Programming
- **主題 / Topic**: TCP Socket 程式設計基礎 / TCP Socket Programming Fundamentals
- **內容 / Contents**: 
  - 客戶端-伺服器通訊程式實作 / Client-Server Communication Program Implementation
  - TCP 連線建立與資料傳輸 / TCP Connection Establishment and Data Transmission
  - Socket API 使用與網路程式設計基礎 / Socket API Usage and Network Programming Fundamentals
- **技術重點 / Technical Focus**:
  - `socket()`, `bind()`, `listen()`, `accept()` 函式 / Functions
  - `connect()`, `send()`, `recv()` 函式 / Functions
  - 網路位址結構 (`sockaddr_in`) / Network Address Structure
  - 錯誤處理與連線管理 / Error Handling and Connection Management
- **狀態 / Status**: ✅ 完成 / Completed

### LAB-2: TCP Echo Server with Timeout Mechanism
- **主題 / Topic**: TCP Echo 伺服器與超時機制 / TCP Echo Server with Timeout Mechanism
- **內容 / Contents**:
  - TCP Echo 伺服器實作 / TCP Echo Server Implementation
  - `select()` 函數實作超時機制 / Timeout Mechanism using `select()` function
  - 輸入驗證與錯誤處理 / Input Validation and Error Handling
  - 字串長度檢查與格式驗證 / String Length Checking and Format Validation
- **技術重點 / Technical Focus**:
  - `select()` 函數用於 I/O 多工與超時控制 / `select()` for I/O Multiplexing and Timeout Control
  - 字串驗證邏輯 / String Validation Logic
  - 伺服器端輸入處理 / Server-side Input Processing
  - 客戶端輸入驗證 / Client-side Input Validation
- **狀態 / Status**: ✅ 完成 / Completed

### LAB-3: Multi-threaded TCP Chatroom
- **主題 / Topic**: 多執行緒 TCP 聊天室 / Multi-threaded TCP Chatroom
- **內容 / Contents**:
  - 多執行緒聊天室系統實作 / Multi-threaded Chatroom System Implementation
  - pthread 程式設計 / Pthread Programming
  - 多聊天室管理（RoomA、RoomB、RoomC）/ Multiple Chatroom Management
  - 客戶端連線數量限制（最多 5 個）/ Client Connection Limit (Maximum 5)
  - 訊息廣播機制 / Message Broadcasting Mechanism
- **技術重點 / Technical Focus**:
  - pthread 執行緒建立與管理 / Pthread Thread Creation and Management
  - pthread_mutex 執行緒同步 / Pthread Mutex Thread Synchronization
  - 共享資源的執行緒安全保護 / Thread-safe Protection of Shared Resources
  - 客戶端連線狀態管理 / Client Connection Status Management
- **狀態 / Status**: ✅ 完成 / Completed

### LAB-4: Multi-Group Dynamic Chatroom (Java)
- **主題 / Topic**: 多群組動態聊天室（Java Socket 程式設計）/ Multi-Group Dynamic Chatroom (Java Socket Programming)
- **內容 / Contents**:
  - Java Socket 程式設計 / Java Socket Programming
  - 多群組動態聊天室系統 / Multi-Group Dynamic Chatroom System
  - 自動補位機制 / Auto-join Mechanism
  - 等待佇列管理 / Waiting Queue Management
  - 訊息歷史記錄 / Message History
  - 全域廣播功能 / Global Broadcast Functionality
- **技術重點 / Technical Focus**:
  - Java `ServerSocket` 與 `Socket` 類別 / Java `ServerSocket` and `Socket` Classes
  - Java 多執行緒（Thread）程式設計 / Java Multi-threading (Thread) Programming
  - `DataInputStream` / `DataOutputStream` 資料傳輸 / Data Transmission using `DataInputStream` / `DataOutputStream`
  - `CopyOnWriteArrayList` 執行緒安全資料結構 / Thread-safe Data Structures using `CopyOnWriteArrayList`
  - 等待佇列與自動補位演算法 / Waiting Queue and Auto-join Algorithm
- **系統參數 / System Parameters**:
  - 伺服器埠號：12345
  - 最大客戶端數：15
  - 群組數量：3（Group_A、Group_B、Group_C）
  - 每個群組最大成員數：4
- **狀態 / Status**: ✅ 完成 / Completed

## 課程進度 / Course Progress

- [x] **LAB-1**: TCP Socket Programming - 完成 / Completed
- [x] **LAB-2**: TCP Echo Server with Timeout - 完成 / Completed
- [x] **LAB-3**: Multi-threaded TCP Chatroom - 完成 / Completed
- [x] **LAB-4**: Multi-Group Dynamic Chatroom (Java) - 完成 / Completed
- [x] **Exam-1**: 第一次上機考試 - 完成 / Completed
- [x] **Exam-2**: 第二次上機考試 - 完成 / Completed
- [x] **Exam-3**: 第三次上機考試 - 完成 / Completed

## 學習目標 / Learning Objectives

1. **理解網路程式設計基礎概念 / Understanding Network Programming Fundamentals**
   - Socket 程式設計模型 / Socket Programming Model
   - TCP/UDP 協定差異 / TCP/UDP Protocol Differences
   - 網路位址與 Port 概念 / Network Address and Port Concepts

2. **掌握 Socket API 使用 / Mastering Socket API Usage**
   - 伺服器端 Socket 建立流程 / Server-side Socket Creation Process
   - 客戶端 Socket 連線流程 / Client Socket Connection Process
   - 資料收發與錯誤處理 / Data Transmission and Error Handling
   - I/O 多工與超時控制 / I/O Multiplexing and Timeout Control

3. **實作網路通訊程式 / Implementing Network Communication Programs**
   - 客戶端-伺服器架構設計 / Client-Server Architecture Design
   - 多執行緒/多程序網路程式 / Multi-threaded/Multi-process Network Programs
   - 執行緒同步與資源管理 / Thread Synchronization and Resource Management
   - 網路安全與效能考量 / Network Security and Performance Considerations

4. **跨語言網路程式設計 / Cross-language Network Programming**
   - C 語言 Socket 程式設計 / C Language Socket Programming
   - Java Socket 程式設計 / Java Socket Programming
   - 不同語言間的通訊實作 / Communication Implementation across Different Languages

## 開發環境 / Development Environment

- **作業系統 / Operating System**: 
  - Debian 13 (Linux) - C 語言開發環境
  - DietPi v9.18.1 - Java 開發環境（LAB-4）
- **編譯器 / Compiler**: 
  - GCC (C 語言) / GCC (C Language)
  - OpenJDK 17 (Java) / OpenJDK 17 (Java)
- **開發工具 / Development Tools**: 
  - 文字編輯器 (Vim/VS Code) / Text Editor
  - 終端機 (Terminal) / Terminal
  - Git 版本控制 / Git Version Control

## 使用說明 / Usage Instructions

### C 語言程式編譯 / Compiling C Programs

```bash
# 進入實驗目錄 / Navigate to lab directory
cd LAB-X/Homework

# 使用編譯腳本（如果有）/ Use compilation script (if available)
chmod +x compile.sh
./compile.sh

# 或手動編譯 / Or compile manually
gcc -o server Server.c
gcc -o client Client.c
```

### Java 程式編譯與執行 / Compiling and Running Java Programs

```bash
# 進入實驗目錄 / Navigate to lab directory
cd LAB-4/Homework

# 編譯 Java 程式 / Compile Java programs
chmod +x Compile.sh
./Compile.sh

# 或手動編譯 / Or compile manually
javac src/Server.java src/Client.java

# 執行伺服器 / Run server
cd src
java Server
# 或使用啟動腳本 / Or use startup script
../server.sh

# 執行客戶端 / Run client
java Client
# 或使用啟動腳本 / Or use startup script
../client.sh
```

### 執行程式 / Running Programs

```bash
# 終端機1: 啟動伺服器 / Terminal 1: Start server
./server

# 終端機2: 啟動客戶端 / Terminal 2: Start client
./client
```

## 學習資源 / Learning Resources

### 參考書籍 / Reference Books
- "Unix Network Programming" by W. Richard Stevens
- "TCP/IP Illustrated" by W. Richard Stevens
- "Linux System Programming" by Robert Love
- "Java Network Programming" by Elliotte Rusty Harold

### 線上資源 / Online Resources
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Linux Socket Programming](https://www.linuxhowtos.org/C_C++/socket.htm)
- [GNU C Library Manual](https://www.gnu.org/software/libc/manual/)
- [Oracle Java Socket Programming Tutorial](https://docs.oracle.com/javase/tutorial/networking/sockets/)
- [Pthread Tutorial](https://hpc-tutorials.llnl.gov/posix/)

## 專案特色 / Project Features

### 程式碼品質 / Code Quality
- 所有程式碼都包含詳細的中文註解 / All code includes detailed Chinese comments
- 完整的錯誤檢查與處理機制 / Complete error checking and handling mechanisms
- 結構化的程式設計與模組化設計 / Structured programming and modular design

### 文件完整性 / Documentation Completeness
- 每個實驗都包含完整的 README 說明文件 / Each lab includes complete README documentation
- 詳細的程式碼講解與系統架構說明 / Detailed code explanations and system architecture descriptions
- 完整的執行結果截圖與功能測試記錄 / Complete execution screenshots and feature test records

### 實作完整性 / Implementation Completeness
- 從基礎 TCP Socket 到進階多執行緒聊天室 / From basic TCP Socket to advanced multi-threaded chatroom
- 涵蓋 C 語言與 Java 兩種語言實作 / Covers implementations in both C and Java languages
- 包含多種網路程式設計技術 / Includes various network programming techniques

## 注意事項 / Important Notes

1. **程式碼規範 / Code Standards**: 所有程式碼都包含詳細的中文註解 / All code includes detailed Chinese comments
2. **錯誤處理 / Error Handling**: 實作完整的錯誤檢查與處理機制 / Implement complete error checking and handling mechanisms
3. **記憶體管理 / Memory Management**: 注意記憶體洩漏與緩衝區溢位問題 / Pay attention to memory leaks and buffer overflow issues
4. **執行緒安全 / Thread Safety**: 多執行緒程式需注意同步與資源競爭問題 / Multi-threaded programs need to pay attention to synchronization and resource competition issues
5. **網路安全 / Network Security**: 考慮基本的網路安全防護措施 / Consider basic network security protection measures

## 聯絡資訊 / Contact Information

- **學生 / Student**: 王建葦 Albert Wang
- **學號 / Student ID**: D1210799
- **Email**: albert@mail.jw-albert.tw

---

**最後更新 / Last Updated**: 2025年01月 / January 2025
**版本 / Version**: 2.0
**授權 / License**: 僅供學習使用 / For Educational Use Only