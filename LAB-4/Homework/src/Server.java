package src;

// ============================================================
// Homework #4 - Multi-Group Dynamic Chatroom Server (Java)
// 使用 DataInputStream / DataOutputStream
// Host: 127.0.0.1, Port: 12345
// ============================================================
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;

/**
 * 多群組動態聊天室伺服器
 * 
 * 功能特性：
 * 1. 支援最多 15 個客戶端同時連線
 * 2. 提供三個聊天群組（Group_A、Group_B、Group_C）
 * 3. 每個群組最多容納 4 位成員
 * 4. 自動補位機制：當群組有空位時，自動將等待佇列中的使用者加入
 * 5. 群組訊息歷史記錄（最多保留 50 條）
 * 6. 全域廣播功能（Server Broadcast）
 * 7. 管理員指令支援（/exit 關閉伺服器）
 */
public class Server {
    // 伺服器監聽埠號
    private static final int PORT = 12345;
    // 伺服器最大同時連線客戶端數量
    private static final int MAX_CLIENTS = 15;
    // 每個群組的最大成員數量
    private static final int MAX_GROUP_SIZE = 4;

    // 儲存各個群組的客戶端處理器列表（Key: 群組名稱, Value: 該群組的 ClientHandler 列表）
    private static final Map<String, List<ClientHandler>> groups = new HashMap<>();
    // 等待加入群組的客戶端佇列（當群組已滿時，使用者會被加入此佇列）
    private static final Queue<ClientHandler> waitingQueue = new LinkedList<>();
    // 所有連線的客戶端列表（使用 CopyOnWriteArrayList 以確保線程安全）
    private static final List<ClientHandler> allClients = new CopyOnWriteArrayList<>();
    // 各群組的訊息歷史記錄（Key: 群組名稱, Value: 訊息列表，最多保留 50 條）
    private static final Map<String, List<String>> groupHistory = new HashMap<>();

    /**
     * 主程式進入點
     * 初始化伺服器、建立群組結構、啟動監聽迴圈
     */
    public static void main(String[] args) {
        // 初始化三個群組的客戶端列表（使用 CopyOnWriteArrayList 確保線程安全）
        groups.put("Group_A", new CopyOnWriteArrayList<>());
        groups.put("Group_B", new CopyOnWriteArrayList<>());
        groups.put("Group_C", new CopyOnWriteArrayList<>());

        // 初始化三個群組的訊息歷史記錄（使用 LinkedList 以便高效插入和刪除）
        groupHistory.put("Group_A", new LinkedList<>());
        groupHistory.put("Group_B", new LinkedList<>());
        groupHistory.put("Group_C", new LinkedList<>());

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Server started on 127.0.0.1:" + PORT);

            // 建立後台執行緒用於處理管理員輸入（系統廣播和關閉指令）
            new Thread(() -> {
                Scanner console = new Scanner(System.in);
                while (true) {
                    String adminMsg = console.nextLine();
                    // 檢查是否為關閉伺服器指令
                    if (adminMsg.equalsIgnoreCase("/exit")) {
                        System.out.println("Server shutting down...");
                        // 廣播伺服器即將關閉的訊息給所有客戶端
                        broadcastAll("[ADMIN] Server Will Close.");
                        // 延遲一秒後關閉伺服器，讓訊息有時間傳送
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        System.exit(0);
                    }
                    // 將管理員訊息廣播給所有客戶端
                    broadcastAll("[ADMIN] " + adminMsg);
                }
            }).start();

            // 主接收迴圈：持續監聽新的客戶端連線請求
            while (true) {
                // 等待客戶端連線（阻塞式）
                Socket socket = serverSocket.accept();

                // 檢查是否已達到最大客戶端數量限制
                if (allClients.size() >= MAX_CLIENTS) {
                    // 建立臨時輸出流告知客戶端伺服器已滿
                    DataOutputStream tempOut = new DataOutputStream(socket.getOutputStream());
                    tempOut.writeUTF("Server is full! Connection rejected.");
                    // 關閉連線
                    socket.close();
                    continue; // 繼續等待下一個連線請求
                }

                // 建立客戶端處理器並加入所有客戶端列表
                ClientHandler client = new ClientHandler(socket);
                allClients.add(client);
                // 啟動客戶端處理執行緒
                client.start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 群組內廣播訊息
     * 將訊息傳送給指定群組的所有成員（除了發送者）
     * 同時將訊息記錄到該群組的歷史記錄中
     * 
     * @param group   目標群組名稱
     * @param message 要廣播的訊息
     * @param sender  發送訊息的客戶端處理器（不會收到自己發送的訊息）
     */
    private static void broadcast(String group, String message, ClientHandler sender) {
        // 遍歷群組內的所有客戶端
        for (ClientHandler c : groups.get(group)) {
            try {
                // 不將訊息發送給發送者本人
                if (c != sender) {
                    c.out.writeUTF(message);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        // 將訊息加入群組歷史記錄
        List<String> history = groupHistory.get(group);
        history.add(message);
        // 如果歷史記錄超過 50 條，移除最舊的記錄（保持 FIFO）
        if (history.size() > 50) {
            history.remove(0);
        }
    }

    /**
     * 全域廣播訊息
     * 將訊息傳送給所有連線的客戶端（無論他們在哪個群組或大廳）
     * 用於系統通知、管理員公告等
     * 
     * @param message 要廣播的訊息
     */
    private static void broadcastAll(String message) {
        // 遍歷所有連線的客戶端
        for (ClientHandler c : allClients) {
            try {
                // 檢查輸出流是否有效（避免已斷線的客戶端造成錯誤）
                if (c.out != null) {
                    // 加上 [Server Broadcast] 前綴以便客戶端識別
                    c.out.writeUTF("[Server Broadcast] " + message);
                }
            } catch (IOException ignored) {
                // 忽略個別客戶端的 IO 錯誤，繼續處理其他客戶端
            }
        }
    }

    /**
     * 更新等待佇列中所有客戶端的排隊位置資訊
     * 當有新客戶端加入等待佇列或佇列順序變動時呼叫
     */
    private static void updateWaitingQueuePositions() {
        int pos = 1;
        // 遍歷等待佇列中的所有客戶端
        for (ClientHandler c : waitingQueue) {
            try {
                // 告知客戶端當前在佇列中的位置
                c.out.writeUTF("You are now in waiting queue position: " + pos);
                pos++;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * 嘗試讓等待佇列中的客戶端自動補進有空位的群組
     * 當有客戶端離開群組時，此方法會被呼叫以自動遞補等待佇列中最早排隊的客戶端
     * 
     * @param groupFreed 有空位的群組名稱
     */
    private static void tryAutoJoin(String groupFreed) {
        // 使用迭代器遍歷等待佇列（支援在遍歷過程中移除元素）
        Iterator<ClientHandler> it = waitingQueue.iterator();
        while (it.hasNext()) {
            ClientHandler c = it.next();
            // 跳過沒有目標群組的客戶端（理論上不應該發生）
            if (c.targetGroup == null)
                continue;
            // 檢查此客戶端是否在等待加入該有空位的群組
            if (c.targetGroup.equals(groupFreed)) {
                List<ClientHandler> g = groups.get(groupFreed);
                // 確認群組仍有空位（雙重檢查，避免競爭條件）
                if (g.size() < MAX_GROUP_SIZE) {
                    // 將客戶端加入群組
                    g.add(c);
                    // 更新客戶端狀態
                    c.currentGroup = groupFreed;
                    c.inLobby = false;
                    // 從等待佇列中移除
                    it.remove();
                    try {
                        // 清空客戶端終端畫面
                        c.out.writeUTF("\033[H\033[2J");
                        // 通知客戶端已自動加入群組
                        c.out.writeUTF("A slot in " + groupFreed + " is now free! You have been auto-joined.\n");
                        // 廣播給群組其他成員：有新成員自動加入
                        broadcast(groupFreed, c.userName + " joined " + groupFreed + " (auto from waiting).", c);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    // 只補一位，找到後即停止
                    break;
                }
            }
        }
        // 更新剩餘等待客戶端的排隊位置資訊
        updateWaitingQueuePositions();
    }

    /**
     * 客戶端處理器類別
     * 每個連線的客戶端都會有一個獨立的 ClientHandler 執行緒
     * 負責處理該客戶端的訊息接收、指令處理、狀態管理等
     */
    private static class ClientHandler extends Thread {
        // 與客戶端的 Socket 連線
        private Socket socket;
        // 資料輸入流（用於接收客戶端訊息）
        private DataInputStream in;
        // 資料輸出流（用於發送訊息給客戶端）
        private DataOutputStream out;
        // 客戶端的使用者名稱（格式：User_1, User_2, ...）
        private String userName;
        // 客戶端當前所在的群組名稱（null 表示在大廳）
        private String currentGroup = null;
        // 客戶端想要加入的目標群組（用於等待佇列機制）
        private String targetGroup = null;
        // 客戶端是否在大廳（true：在大廳，false：在群組內）
        private boolean inLobby = true;

        /**
         * 建構子：初始化客戶端處理器
         * 
         * @param socket 與客戶端的 Socket 連線
         */
        public ClientHandler(Socket socket) {
            this.socket = socket;
        }

        /**
         * 客戶端處理執行緒的主迴圈
         * 負責接收客戶端訊息並進行相應處理
         */
        public void run() {
            try {
                // 建立資料輸入輸出流
                in = new DataInputStream(socket.getInputStream());
                out = new DataOutputStream(socket.getOutputStream());
                // 根據在 allClients 列表中的位置分配使用者名稱
                userName = "User_" + (allClients.indexOf(this) + 1);

                // 將客戶端加入大廳（顯示歡迎訊息和可用指令）
                enterLobby();

                // 廣播新客戶端連線訊息給所有線上客戶端
                // 注意：必須在 out 建立後才能廣播，否則會出錯
                broadcastAll(userName + " has connected to the server.");

                // 主訊息處理迴圈：持續接收並處理客戶端訊息
                while (true) {
                    // 從客戶端讀取訊息（阻塞式，直到收到訊息）
                    String msg = in.readUTF();

                    // 處理 EXIT! 指令：客戶端主動退出
                    if (msg.equalsIgnoreCase("EXIT!")) {
                        handleExit();
                        break;
                    }
                    // 處理 JOIN_Group_X 指令：加入指定群組
                    else if (msg.startsWith("JOIN_")) {
                        handleJoin(msg.substring(5)); // 提取群組名稱
                    }
                    // 處理 CHANGE_Group_X 指令：切換到另一個群組
                    else if (msg.startsWith("CHANGE_")) {
                        handleChange(msg.substring(7)); // 提取新群組名稱
                    }
                    // 處理 LEAVE 指令：離開當前群組回到大廳
                    else if (msg.equalsIgnoreCase("LEAVE")) {
                        handleLeave();
                    }
                    // 處理 HISTORY 指令：查看當前群組的訊息歷史
                    else if (msg.equalsIgnoreCase("HISTORY")) {
                        handleHistory();
                    }
                    // 處理 BROADCAST 指令：全域廣播訊息（給所有客戶端）
                    else if (msg.startsWith("BROADCAST ")) {
                        broadcastAll("[User Broadcast] " + userName + ": " + msg.substring(10));
                    }
                    // 如果客戶端在群組內，將訊息作為聊天訊息廣播到群組
                    else if (currentGroup != null) {
                        String formatted = "[" + userName + "]: " + msg;
                        broadcast(currentGroup, formatted, this);
                    }
                    // 如果客戶端不在群組內，提示需要先加入群組
                    else {
                        out.writeUTF("You are not in any group. Use JOIN_Group_X to chat.");
                    }
                }
            } catch (IOException e) {
                // 當連線中斷時（客戶端異常斷線），處理斷線邏輯
                handleDisconnect();
            }
        }

        /**
         * 將客戶端加入大廳
         * 清空畫面並顯示歡迎訊息和可用指令列表
         */
        private void enterLobby() throws IOException {
            inLobby = true;
            // ANSI 清屏指令：\033[H 移動游標到左上角，\033[2J 清除整個畫面
            out.writeUTF("\033[H\033[2J");
            // 顯示歡迎訊息和所有可用指令
            out.writeUTF("Welcome " + userName + " to the Lobby.\nAvailable commands:\n"
                    + "JOIN_Group_A / JOIN_Group_B / JOIN_Group_C\n"
                    + "CHANGE_Group_A / CHANGE_Group_B / CHANGE_Group_C\n"
                    + "LEAVE / HISTORY / EXIT!\n");
        }

        /**
         * 處理 JOIN_Group_X 指令
         * 將客戶端加入指定群組，如果群組已滿則加入等待佇列
         * 
         * @param groupName 群組名稱（例如："Group_A" 或 "A"）
         */
        private void handleJoin(String groupName) throws IOException {
            // 正規化群組名稱：移除可能的重複前綴，轉換為大寫，加上 Group_ 前綴
            String group = "Group_" + groupName.replace("Group_", "").toUpperCase();
            // 驗證群組名稱是否有效
            if (!groups.containsKey(group)) {
                out.writeUTF("Invalid group name!");
                return;
            }
            // 記錄目標群組（用於等待佇列機制）
            targetGroup = group;

            // 取得目標群組的客戶端列表
            List<ClientHandler> g = groups.get(group);
            // 檢查群組是否已滿
            if (g.size() >= MAX_GROUP_SIZE) {
                // 將客戶端加入等待佇列
                waitingQueue.add(this);
                // 告知客戶端群組已滿並顯示在等待佇列中的位置
                out.writeUTF(group + " is full! You are added to waiting queue position: " + waitingQueue.size());
                // 更新所有等待客戶端的排隊位置資訊
                updateWaitingQueuePositions();
                return;
            }

            // 群組有空位，直接加入
            g.add(this);
            currentGroup = group;
            inLobby = false;
            // 廣播給群組其他成員：有新成員加入
            broadcast(group, userName + " joined " + group + ".", this);
            // 告知客戶端已成功加入群組
            out.writeUTF("You joined " + group + ".");
        }

        /**
         * 處理 CHANGE_Group_X 指令
         * 將客戶端從當前群組切換到另一個群組
         * 
         * @param groupName 目標群組名稱
         */
        private void handleChange(String groupName) throws IOException {
            // 檢查客戶端是否在群組內
            if (currentGroup == null) {
                out.writeUTF("You are not in a group. Use JOIN_ first.");
                return;
            }

            // 正規化目標群組名稱
            String newGroup = "Group_" + groupName.replace("Group_", "").toUpperCase();
            // 驗證群組名稱是否有效
            if (!groups.containsKey(newGroup)) {
                out.writeUTF("Invalid group name!");
                return;
            }

            // 檢查目標群組是否已滿
            if (groups.get(newGroup).size() >= MAX_GROUP_SIZE) {
                out.writeUTF(newGroup + " is full! You remain in " + currentGroup + ".");
                return;
            }

            // 從原群組中移除
            groups.get(currentGroup).remove(this);
            // 廣播給原群組成員：客戶端已離開
            broadcast(currentGroup, userName + " left " + currentGroup + ".", this);

            // 加入新群組
            currentGroup = newGroup;
            groups.get(newGroup).add(this);
            // 廣播給新群組成員：客戶端已加入
            broadcast(newGroup, userName + " joined " + newGroup + ".", this);
            // 告知客戶端已成功切換群組
            out.writeUTF("You switched to " + newGroup + ".");
        }

        /**
         * 處理 LEAVE 指令
         * 讓客戶端離開當前群組並返回大廳
         * 離開後會觸發自動補位機制
         */
        private void handleLeave() throws IOException {
            if (currentGroup != null) {
                // 保存離開的群組名稱（用於自動補位）
                String leftGroup = currentGroup;
                // 從群組中移除客戶端
                groups.get(currentGroup).remove(this);
                // 廣播給群組其他成員：客戶端已離開
                broadcast(leftGroup, userName + " left " + leftGroup + ".", this);
                // 清除當前群組狀態
                currentGroup = null;
                // 清空畫面
                out.writeUTF("\033[H\033[2J");
                // 告知客戶端已離開群組並返回大廳
                out.writeUTF("You have left the group and returned to the Lobby.");
                // 嘗試讓等待佇列中的客戶端自動補進該群組
                tryAutoJoin(leftGroup);
                // 重新進入大廳（顯示歡迎訊息）
                enterLobby();
            } else {
                out.writeUTF("You are not in a group.");
            }
        }

        /**
         * 處理 HISTORY 指令
         * 顯示當前群組的訊息歷史記錄（最多 50 條）
         */
        private void handleHistory() throws IOException {
            // 檢查客戶端是否在群組內
            if (currentGroup == null) {
                out.writeUTF("You are not in a group!");
                return;
            }
            // 發送歷史記錄標題
            out.writeUTF("=== Last 50 Messages of " + currentGroup + " ===");
            // 遍歷並發送所有歷史記錄
            for (String line : groupHistory.get(currentGroup)) {
                out.writeUTF(line);
            }
            // 發送結束標記
            out.writeUTF("=== End of History ===");
        }

        /**
         * 處理 EXIT! 指令
         * 客戶端主動退出，清理資源並通知其他客戶端
         */
        private void handleExit() throws IOException {
            // 如果客戶端在群組內，先從群組中移除
            if (currentGroup != null) {
                String leftGroup = currentGroup;
                groups.get(leftGroup).remove(this);
                // 廣播給群組其他成員：客戶端已離開
                broadcast(leftGroup, userName + " left " + leftGroup + ".", this);
                // 嘗試自動補位
                tryAutoJoin(leftGroup);
            }
            // 從所有客戶端列表中移除
            allClients.remove(this);
            // 全域廣播：客戶端已離開伺服器
            broadcastAll(userName + " has left the server.");
            // 關閉連線
            closeConnection();
        }

        /**
         * 處理客戶端異常斷線
         * 當連線中斷時（IOException）自動呼叫此方法
         */
        private void handleDisconnect() {
            try {
                // 保存離開的群組名稱
                String leftGroup = currentGroup;
                // 關閉連線
                closeConnection();
                // 從所有客戶端列表中移除
                allClients.remove(this);
                // 如果客戶端在群組內，通知群組其他成員
                if (leftGroup != null) {
                    broadcast(leftGroup, userName + " disconnected.", this);
                }
                // 在伺服器端顯示斷線訊息
                System.out.println(userName + " disconnected.");
                // 如果客戶端在群組內，觸發自動補位
                if (leftGroup != null) {
                    tryAutoJoin(leftGroup);
                }
            } catch (Exception ignored) {
                // 忽略清理過程中的錯誤
            }
        }

        /**
         * 關閉與客戶端的 Socket 連線
         */
        private void closeConnection() throws IOException {
            socket.close();
        }
    }
}