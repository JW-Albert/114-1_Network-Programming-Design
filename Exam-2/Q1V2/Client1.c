#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5678
#define BUFFER_SIZE 256

int sock;

// 接收 Server 訊息的執行緒
void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (read_size <= 0) {
            printf("\nServer disconnected.\n");
            close(sock);
            exit(0);
        }
        buffer[read_size] = '\0';
        printf("\n[Server]: %s\n", buffer);
        printf("[You]: ");
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char student_id[50], account[50], password[50];
    char input_account[50], input_password[50];
    char msg[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(1);
    }

    printf("Connected to server.\n");

    // ===== 註冊階段 =====
    printf("=== Registration ===\n");
    printf("Enter student ID: ");
    fgets(student_id, sizeof(student_id), stdin);
    student_id[strcspn(student_id, "\n")] = '\0';

    printf("Enter new account: ");
    fgets(account, sizeof(account), stdin);
    account[strcspn(account, "\n")] = '\0';

    do {
        printf("Enter new password (6~15 chars): ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = '\0';
        if (strlen(password) < 6 || strlen(password) > 15)
            printf("Invalid password length.\n");
    } while (strlen(password) < 6 || strlen(password) > 15);

    send(sock, student_id, strlen(student_id), 0);
    send(sock, account, strlen(account), 0);
    send(sock, password, strlen(password), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    // ===== 登入階段 =====
    printf("\n=== Login ===\n");
    printf("Enter account: ");
    fgets(input_account, sizeof(input_account), stdin);
    input_account[strcspn(input_account, "\n")] = '\0';

    printf("Enter password: ");
    fgets(input_password, sizeof(input_password), stdin);
    input_password[strcspn(input_password, "\n")] = '\0';

    send(sock, input_account, strlen(input_account), 0);
    send(sock, input_password, strlen(input_password), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    if (strncmp(buffer, "Wrong", 5) == 0) {
        close(sock);
        return 0;
    }

    // ===== 聊天階段 =====
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    printf("\n=== Chatting Mode ===\n");
    while (1) {
        memset(msg, 0, sizeof(msg));
        if (fgets(msg, sizeof(msg), stdin) == NULL) break;
        msg[strcspn(msg, "\n")] = '\0';
        if (strlen(msg) == 0) continue;
        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
    return 0;
}
