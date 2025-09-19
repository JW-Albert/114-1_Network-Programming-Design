# 網路程式設計課程紀錄 / Network Programming Design Course Record

## 課程資訊 / Course Information
- **學校 / University**: 逢甲大學 Feng Chia University
- **課程名稱 / Course Name**: 網路程式設計 (Network Programming Design)
- **學期 / Semester**: 114-1
- **學生 / Student**: 王建葦 (D1210799)
- **班級 / Class**: 資訊三乙 (IECS 3B)
- **授課教師 / Instructor**: 陳烈武

## Repository 簡介 / Repository Introduction

本 repository 記錄了網路程式設計課程的所有實驗作業、程式碼實作與學習筆記。內容包含 TCP/UDP Socket 程式設計、網路通訊協定實作、以及相關的網路程式設計技術。

This repository documents all laboratory assignments, code implementations, and learning notes for the Network Programming Design course. It includes TCP/UDP Socket programming, network communication protocol implementations, and related network programming technologies.

## 目錄結構 / Directory Structure

```
114-1_Network-Programming-Design/
├── README.md                           # 本文件 / This file
├── LAB-1/                             # 實驗一：TCP Socket Programming / Lab 1: TCP Socket Programming
│   ├── Homework/                      # 作業檔案 / Assignment Files
│   │   ├── README.md                  # 作業說明文件 / Assignment Documentation
│   │   ├── README_EN.md              # 英文版說明文件 / English Documentation
│   │   ├── Client.c                   # 客戶端程式碼 / Client Code
│   │   ├── Server.c                   # 伺服器程式碼 / Server Code
│   │   ├── Demo.PNG                   # 執行結果截圖 / Execution Screenshot
│   │   └── compile.sh                 # 編譯腳本 (選擇性) / Compilation Script (Optional)
│   ├── Sample/                        # 範例程式碼 / Sample Code
│   │   ├── Client.c                   # 範例客戶端 / Sample Client
│   │   ├── Server.c                   # 範例伺服器 / Sample Server
│   │   ├── client                     # 編譯後的客戶端執行檔 / Compiled Client Executable
│   │   ├── server                     # 編譯後的伺服器執行檔 / Compiled Server Executable
│   │   └── com.sh                     # 執行腳本 / Execution Script
│   └── LAB1-TCP Socket Programming in Linux.pptx  # 實驗講義 / Lab Materials
└── [其他實驗目錄...]                   # 後續實驗將陸續新增 / Additional labs will be added
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

## 學習目標 / Learning Objectives

1. **理解網路程式設計基礎概念 / Understanding Network Programming Fundamentals**
   - Socket 程式設計模型 / Socket Programming Model
   - TCP/UDP 協定差異 / TCP/UDP Protocol Differences
   - 網路位址與 Port 概念 / Network Address and Port Concepts

2. **掌握 Socket API 使用 / Mastering Socket API Usage**
   - 伺服器端 Socket 建立流程 / Server-side Socket Creation Process
   - 客戶端 Socket 連線流程 / Client Socket Connection Process
   - 資料收發與錯誤處理 / Data Transmission and Error Handling

3. **實作網路通訊程式 / Implementing Network Communication Programs**
   - 客戶端-伺服器架構設計 / Client-Server Architecture Design
   - 多執行緒/多程序網路程式 / Multi-threaded/Multi-process Network Programs
   - 網路安全與效能考量 / Network Security and Performance Considerations

## 開發環境 / Development Environment

- **作業系統 / Operating System**: Debian 13 (Linux)
- **編譯器 / Compiler**: GCC
- **開發工具 / Development Tools**: 
  - 文字編輯器 (Vim/VS Code) / Text Editor
  - 終端機 (Terminal) / Terminal
  - Git 版本控制 / Git Version Control

## 使用說明 / Usage Instructions

### 編譯程式 / Compiling Programs
```bash
# 進入實驗目錄 / Navigate to lab directory
cd LAB-1/Homework

# 編譯伺服器程式 / Compile server program
gcc -o server Server.c

# 編譯客戶端程式 / Compile client program
gcc -o client Client.c
```

### 執行程式 / Running Programs
```bash
# 終端機1: 啟動伺服器 / Terminal 1: Start server
./server

# 終端機2: 啟動客戶端 / Terminal 2: Start client
./client
```

## 課程進度 / Course Progress

- [x] **LAB-1**: TCP Socket Programming - 完成 / Completed
- [ ] **LAB-2**: UDP Socket Programming - 進行中 / In Progress
- [ ] **LAB-3**: Multi-threaded Network Programming - 待開始 / Pending
- [ ] **LAB-4**: Network Security Programming - 待開始 / Pending

## 學習資源 / Learning Resources

### 參考書籍 / Reference Books
- "Unix Network Programming" by W. Richard Stevens
- "TCP/IP Illustrated" by W. Richard Stevens
- "Linux System Programming" by Robert Love

### 線上資源 / Online Resources
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Linux Socket Programming](https://www.linuxhowtos.org/C_C++/socket.htm)
- [GNU C Library Manual](https://www.gnu.org/software/libc/manual/)

## 注意事項 / Important Notes

1. **程式碼規範 / Code Standards**: 所有程式碼都包含詳細的中文註解 / All code includes detailed Chinese comments
2. **錯誤處理 / Error Handling**: 實作完整的錯誤檢查與處理機制 / Implement complete error checking and handling mechanisms
3. **記憶體管理 / Memory Management**: 注意記憶體洩漏與緩衝區溢位問題 / Pay attention to memory leaks and buffer overflow issues
4. **網路安全 / Network Security**: 考慮基本的網路安全防護措施 / Consider basic network security protection measures

## 聯絡資訊 / Contact Information

- **學生 / Student**: 王建葦 Albert Wang
- **學號 / Student ID**: D1210799
- **Email**: albert@mail.jw-albert.tw

---

**最後更新 / Last Updated**: 2025年09月 / September 2025  
**版本 / Version**: 1.0  
**授權 / License**: 僅供學習使用 / For Educational Use Only