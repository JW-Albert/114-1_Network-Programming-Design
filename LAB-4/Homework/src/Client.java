package src;

// ============================================================
// Homework #4 - Multi-Group Dynamic Chatroom Client (Java)
// 使用 DataInputStream / DataOutputStream
// Host: 127.0.0.1, Port: 12345
// ============================================================
import java.io.*;
import java.net.*;

/**
 * 多群組動態聊天室客戶端
 * 
 * 功能特性：
 * 1. 連線到伺服器並建立雙向通訊
 * 2. 使用獨立執行緒接收伺服器訊息（非阻塞）
 * 3. 主執行緒負責讀取使用者輸入並傳送至伺服器
 * 4. 支援所有伺服器指令（JOIN、CHANGE、LEAVE、HISTORY、EXIT!）
 */
public class Client {
    // 伺服器主機位址（本地主機）
    private static final String HOST = "127.0.0.1";
    // 伺服器監聽埠號
    private static final int PORT = 12345;

    /**
     * 主程式進入點
     * 建立與伺服器的連線，啟動訊息接收執行緒，處理使用者輸入
     */
    public static void main(String[] args) {
        try (Socket socket = new Socket(HOST, PORT)) {
            System.out.println("Connected to server at " + HOST + ":" + PORT);

            // 建立資料輸入輸出流
            // DataInputStream 用於接收伺服器傳送的 UTF-8 編碼字串
            DataInputStream in = new DataInputStream(socket.getInputStream());
            // DataOutputStream 用於發送 UTF-8 編碼字串至伺服器
            DataOutputStream out = new DataOutputStream(socket.getOutputStream());

            // 建立訊息接收執行緒（獨立執行緒處理伺服器訊息）
            // 使用獨立執行緒避免阻塞使用者輸入，讓程式能同時接收訊息和處理輸入
            Thread receiveThread = new Thread(() -> {
                try {
                    // 持續接收伺服器訊息（阻塞式讀取）
                    while (true) {
                        // 從伺服器讀取 UTF-8 編碼的字串訊息
                        String msg = in.readUTF();
                        // 將接收到的訊息輸出到終端
                        System.out.println(msg);
                    }
                } catch (IOException e) {
                    // 當連線中斷時（伺服器關閉或網路錯誤），顯示斷線訊息
                    System.out.println("Disconnected from server.");
                }
            });
            // 啟動接收執行緒（開始在背景執行）
            receiveThread.start();

            // 主執行緒處理使用者輸入和訊息傳送
            // 使用 BufferedReader 讀取標準輸入（鍵盤輸入）
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            String msg;
            // 持續讀取使用者輸入（每行視為一條訊息）
            while ((msg = reader.readLine()) != null) {
                // 將使用者輸入的字串以 UTF-8 編碼發送至伺服器
                out.writeUTF(msg);
                // 如果使用者輸入 EXIT!，結束迴圈並關閉連線
                if (msg.equalsIgnoreCase("EXIT!")) {
                    break;
                }
            }

        } catch (IOException e) {
            // 處理連線失敗的情況（伺服器未啟動、網路問題等）
            System.out.println("Connection failed: " + e.getMessage());
        }
        // try-with-resources 會自動關閉 Socket，無需手動關閉
    }
}