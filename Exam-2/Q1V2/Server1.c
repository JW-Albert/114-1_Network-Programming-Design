#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5678
#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char student_id[50], account[50], password[50];
    char input_account[50], input_password[50];
    char client_name[50];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    listen(server_fd, 5);
    printf("Server started on 127.0.0.1:%d\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // === 註冊階段 ===
    recv(client_fd, student_id, sizeof(student_id), 0);
    recv(client_fd, account, sizeof(account), 0);
    recv(client_fd, password, sizeof(password), 0);
    printf("[REGISTER] ID:%s  Account:%s  Password:%s\n", student_id, account, password);

    strcpy(buffer, "Registration successful. Please login now.");
    send(client_fd, buffer, strlen(buffer), 0);

    // === 登入階段 ===
    recv(client_fd, input_account, sizeof(input_account), 0);
    recv(client_fd, input_password, sizeof(input_password), 0);

    if (strcmp(account, input_account) != 0) {
        strcpy(buffer, "Wrong ID!!!");
        send(client_fd, buffer, strlen(buffer), 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    if (strcmp(password, input_password) != 0) {
        strcpy(buffer, "Wrong Password!!!");
        send(client_fd, buffer, strlen(buffer), 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    strcpy(buffer, "Login successful. You can chat now!");
    send(client_fd, buffer, strlen(buffer), 0);
    strcpy(client_name, account);

    printf("Client (%s) logged in successfully!\n", client_name);

    // === 聊天階段 ===
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (read_size <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        buffer[read_size] = '\0';
        printf("[%s]: %s\n", client_name, buffer);

        printf("[You]: ");
        fflush(stdout);
        char msg[BUFFER_SIZE];
        if (fgets(msg, sizeof(msg), stdin) == NULL) break;
        msg[strcspn(msg, "\n")] = '\0';
        send(client_fd, msg, strlen(msg), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
