// ============================================================
// Homework #4 - Multi-Group Dynamic Chatroom Server (Java)
// 使用 DataInputStream / DataOutputStream
// Host: 127.0.0.1, Port: 12345
// ============================================================
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;

public class Server {
    private static final int PORT = 12345;
    private static final int MAX_CLIENTS = 15;
    private static final int MAX_GROUP_SIZE = 4;

    // 三個群組
    private static final Map<String, List<ClientHandler>> groups = new HashMap<>();
    // 等待隊列
    private static final Queue<ClientHandler> waitingQueue = new LinkedList<>();
    // 所有連線的 client
    private static final List<ClientHandler> allClients = new CopyOnWriteArrayList<>();

    // 每個群組的歷史訊息
    private static final Map<String, List<String>> groupHistory = new HashMap<>();

    public static void main(String[] args) {
        groups.put("Group_A", new CopyOnWriteArrayList<>());
        groups.put("Group_B", new CopyOnWriteArrayList<>());
        groups.put("Group_C", new CopyOnWriteArrayList<>());

        groupHistory.put("Group_A", new LinkedList<>());
        groupHistory.put("Group_B", new LinkedList<>());
        groupHistory.put("Group_C", new LinkedList<>());

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Server started on 127.0.0.1:" + PORT);

            while (true) {
                Socket socket = serverSocket.accept();

                // 超過 15 人直接拒絕
                if (allClients.size() >= MAX_CLIENTS) {
                    DataOutputStream tempOut = new DataOutputStream(socket.getOutputStream());
                    tempOut.writeUTF("Server is full! Connection rejected.");
                    socket.close();
                    continue;
                }

                ClientHandler client = new ClientHandler(socket);
                allClients.add(client);
                client.start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 傳送訊息給群組內所有成員
    private static void broadcast(String group, String message, ClientHandler sender) {
        for (ClientHandler c : groups.get(group)) {
            try {
                if (c != sender) {
                    c.out.writeUTF(message);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        // 儲存歷史訊息
        List<String> history = groupHistory.get(group);
        history.add(message);
        if (history.size() > 50) history.remove(0);
    }

    // 通知所有等待中的使用者他們的目前順位
    private static void updateWaitingQueuePositions() {
        int position = 1;
        for (ClientHandler c : waitingQueue) {
            try {
                c.out.writeUTF("You are now in waiting queue position: " + position);
                position++;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    // client handler 類別
    private static class ClientHandler extends Thread {
        private Socket socket;
        private DataInputStream in;
        private DataOutputStream out;
        private String userName;
        private String currentGroup = null;
        private boolean inLobby = true;

        public ClientHandler(Socket socket) {
            this.socket = socket;
        }

        public void run() {
            try {
                in = new DataInputStream(socket.getInputStream());
                out = new DataOutputStream(socket.getOutputStream());

                userName = "User_" + (allClients.indexOf(this) + 1);

                // 若群組與等待都滿 → 直接拒絕
                int total = groups.get("Group_A").size() + groups.get("Group_B").size() + groups.get("Group_C").size() + waitingQueue.size();
                if (total >= MAX_CLIENTS) {
                    out.writeUTF("Server is full! Try again later.");
                    closeConnection();
                    return;
                }

                // 若群組滿且等待還有空間 → 加入等待隊列
                if (total >= MAX_GROUP_SIZE * 3) {
                    waitingQueue.add(this);
                    out.writeUTF("All groups full. You are in waiting queue position: " + waitingQueue.size());
                    inLobby = false;
                } else {
                    enterLobby();
                }

                while (true) {
                    String msg = in.readUTF();

                    if (msg.equalsIgnoreCase("EXIT!")) {
                        handleExit();
                        break;
                    } else if (msg.startsWith("JOIN_")) {
                        handleJoin(msg.substring(5));
                    } else if (msg.startsWith("CHANGE_")) {
                        handleChange(msg.substring(7));
                    } else if (msg.equalsIgnoreCase("LEAVE")) {
                        handleLeave();
                    } else if (msg.equalsIgnoreCase("HISTORY")) {
                        handleHistory();
                    } else if (currentGroup != null) {
                        String formatted = "[" + userName + "]: " + msg;
                        broadcast(currentGroup, formatted, this);
                    } else {
                        out.writeUTF("You are not in any group. Use JOIN_Group_X to chat.");
                    }
                }
            } catch (IOException e) {
                handleDisconnect();
            }
        }

        private void enterLobby() throws IOException {
            inLobby = true;
            out.writeUTF("Welcome " + userName + " to the Lobby.\nAvailable commands:\n"
                    + "JOIN_Group_A / JOIN_Group_B / JOIN_Group_C\n"
                    + "CHANGE_Group_A / CHANGE_Group_B / CHANGE_Group_C\n"
                    + "LEAVE / HISTORY / EXIT!\n");
        }

        private void handleJoin(String groupName) throws IOException {
            String group = "Group_" + groupName.replace("Group_", "").toUpperCase();

            if (!groups.containsKey(group)) {
                out.writeUTF("Invalid group name!");
                return;
            }

            // 如果群組滿，加入等待佇列
            if (groups.get(group).size() >= MAX_GROUP_SIZE) {
                waitingQueue.add(this);
                int pos = waitingQueue.size();
                out.writeUTF(group + " is full! You are added to waiting queue position: " + pos);
                updateWaitingQueuePositions();
                return;
            }

            // 正常加入群組
            groups.get(group).add(this);
            currentGroup = group;
            inLobby = false;

            broadcast(group, userName + " joined " + group + ".", this);
            out.writeUTF("You joined " + group + ".");
        }

        private void handleChange(String groupName) throws IOException {
            if (currentGroup == null) {
                out.writeUTF("You are not in a group. Use JOIN_ first.");
                return;
            }

            String newGroup = "Group_" + groupName.replace("Group_", "").toUpperCase();
            if (!groups.containsKey(newGroup)) {
                out.writeUTF("Invalid group name!");
                return;
            }

            if (groups.get(newGroup).size() >= MAX_GROUP_SIZE) {
                out.writeUTF(newGroup + " is full!");
                return;
            }

            groups.get(currentGroup).remove(this);
            broadcast(currentGroup, userName + " left " + currentGroup + ".", this);

            currentGroup = newGroup;
            groups.get(currentGroup).add(this);
            broadcast(currentGroup, userName + " joined " + currentGroup + ".", this);
            out.writeUTF("You switched to " + currentGroup + ".");
        }

        private void handleLeave() throws IOException {
            if (currentGroup != null) {
                groups.get(currentGroup).remove(this);
                broadcast(currentGroup, userName + " left " + currentGroup + ".", this);
                currentGroup = null;
            }

            waitingQueue.add(this);
            out.writeUTF("You have left the group. Added to waiting queue position: " + waitingQueue.size());

            updateWaitingQueuePositions();

            if (!waitingQueue.isEmpty()) {
                ClientHandler next = waitingQueue.poll();
                if (next != null) next.enterLobby();
            }
        }

        private void handleHistory() throws IOException {
            if (currentGroup == null) {
                out.writeUTF("You are not in a group!");
                return;
            }
            out.writeUTF("=== Last 50 Messages of " + currentGroup + " ===");
            for (String line : groupHistory.get(currentGroup)) {
                out.writeUTF(line);
            }
            out.writeUTF("=== End of History ===");
        }

        private void handleExit() throws IOException {
            if (currentGroup != null) {
                groups.get(currentGroup).remove(this);
                broadcast(currentGroup, userName + " left " + currentGroup + ".", this);
            }
            allClients.remove(this);
            updateWaitingQueuePositions();
            closeConnection();
        }

        private void handleDisconnect() {
            try {
                closeConnection();
                allClients.remove(this);
                if (currentGroup != null)
                    broadcast(currentGroup, userName + " disconnected.", this);
                System.out.println(userName + " disconnected.");
                updateWaitingQueuePositions();
            } catch (Exception ignored) {}
        }

        private void closeConnection() throws IOException {
            socket.close();
        }
    }
}