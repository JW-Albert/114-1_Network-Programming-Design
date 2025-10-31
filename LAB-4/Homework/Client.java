// ============================================================
// Homework #4 - Multi-Group Dynamic Chatroom Client (Java)
// 使用 DataInputStream / DataOutputStream
// Host: 127.0.0.1, Port: 12345
// ============================================================
import java.io.*;
import java.net.*;

public class Client {
    private static final String HOST = "127.0.0.1";
    private static final int PORT = 12345;

    public static void main(String[] args) {
        try (Socket socket = new Socket(HOST, PORT)) {
            System.out.println("Connected to server at " + HOST + ":" + PORT);

            DataInputStream in = new DataInputStream(socket.getInputStream());
            DataOutputStream out = new DataOutputStream(socket.getOutputStream());

            // 接收訊息執行緒
            Thread receiveThread = new Thread(() -> {
                try {
                    while (true) {
                        String msg = in.readUTF();
                        System.out.println(msg);
                    }
                } catch (IOException e) {
                    System.out.println("Disconnected from server.");
                }
            });
            receiveThread.start();

            // 傳送訊息
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            String msg;
            while ((msg = reader.readLine()) != null) {
                out.writeUTF(msg);
                if (msg.equalsIgnoreCase("EXIT!")) break;
            }

        } catch (IOException e) {
            System.out.println("Connection failed: " + e.getMessage());
        }
    }
}