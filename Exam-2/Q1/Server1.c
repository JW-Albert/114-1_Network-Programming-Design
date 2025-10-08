#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char studentID[BUFFER_SIZE], username[BUFFER_SIZE], password[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE], sendbuf[BUFFER_SIZE];
    char login_user[BUFFER_SIZE], login_pass[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5678);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // ====== 註冊階段 ======
    recv(client_fd, studentID, sizeof(studentID), 0);
    recv(client_fd, username, sizeof(username), 0);
    recv(client_fd, password, sizeof(password), 0);
    printf("註冊成功: 學號=%s, 帳號=%s, 密碼=%s\n", studentID, username, password);

    // ====== 登入階段 ======
    send(client_fd, "Please login\n", 13, 0);
    recv(client_fd, login_user, sizeof(login_user), 0);
    recv(client_fd, login_pass, sizeof(login_pass), 0);

    if (strcmp(login_user, username) != 0) {
        send(client_fd, "Wrong ID!!!\n", 12, 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }
    if (strcmp(login_pass, password) != 0) {
        send(client_fd, "Wrong Password!!!\n", 18, 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    send(client_fd, "Login success! Start chatting.\n", 31, 0);
    printf("Client logged in successfully!\n");

    // ====== 聊天階段 ======
    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(client_fd, recvbuf, sizeof(recvbuf), 0);
        if (len <= 0) break;
        recvbuf[len] = '\0';
        if (strcmp(recvbuf, "exit") == 0) break;
        printf("Client: %s\n", recvbuf);

        sprintf(sendbuf, "Server reply: %s", recvbuf);
        send(client_fd, sendbuf, strlen(sendbuf), 0);
    }

    printf("Client disconnected.\n");
    close(client_fd);
    close(server_fd);
    return 0;
}
