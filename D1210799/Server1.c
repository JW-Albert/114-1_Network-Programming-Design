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
    char recvbuf[BUFFER_SIZE * 3];
    char studentID[BUFFER_SIZE], username[BUFFER_SIZE], password[BUFFER_SIZE];
    char login_user[BUFFER_SIZE], login_pass[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5678);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    memset(recvbuf, 0, sizeof(recvbuf));
    int len = recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
    if (len <= 0) {
        close(client_fd);
        close(server_fd);
        return 0;
    }

    char *token = strtok(recvbuf, "|");
    if (token)
        strcpy(studentID, token);
    token = strtok(NULL, "|");
    if (token)
        strcpy(username, token);
    token = strtok(NULL, "|");
    if (token)
        strcpy(password, token);

    printf("註冊成功: 學號=%s, 帳號=%s, 密碼=%s\n", studentID, username, password);

    char msg1[] = "Please login\n";
    send(client_fd, msg1, strlen(msg1), 0);

    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        len = recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0) {
            close(client_fd);
            close(server_fd);
            return 0;
        }

        token = strtok(recvbuf, "|");
        if (token) {
            strcpy(login_user, token);
        }
        token = strtok(NULL, "|");
        if (token) {
            strcpy(login_pass, token);
        }

        if (strcmp(login_user, username) != 0) {
            char msg2[] = "Wrong ID!!!\n";
            send(client_fd, msg2, strlen(msg2), 0);
            continue; // 重新嘗試登入
        }
        if (strcmp(login_pass, password) != 0) {
            char msg3[] = "Wrong Password!!!\n";
            send(client_fd, msg3, strlen(msg3), 0);
            continue; // 重新嘗試登入
        }

        char msg4[] = "Login success! Start chatting.\n";
        send(client_fd, msg4, strlen(msg4), 0);
        printf("Client logged in successfully! (%s)\n", username);
        break;
    }

    fd_set fds;
    char msgbuf[BUFFER_SIZE];
    while (1) {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(client_fd, &fds);
        int maxfd = client_fd + 1;
        select(maxfd, &fds, NULL, NULL, NULL);

        if (FD_ISSET(client_fd, &fds)) {
            memset(msgbuf, 0, sizeof(msgbuf));
            int r = recv(client_fd, msgbuf, sizeof(msgbuf) - 1, 0);
            if (r <= 0)
                break;
            msgbuf[r] = '\0';
            if (strcmp(msgbuf, "exit") == 0)
                break;
            printf("[%s]: %s\n", username, msgbuf);
        }

        if (FD_ISSET(0, &fds)) {
            memset(sendbuf, 0, sizeof(sendbuf));
            fgets(sendbuf, sizeof(sendbuf), stdin);
            sendbuf[strcspn(sendbuf, "\n")] = 0;
            send(client_fd, sendbuf, strlen(sendbuf), 0);
            if (strcmp(sendbuf, "exit") == 0)
                break;
        }
    }

    printf("Client disconnected.\n");
    close(client_fd);
    close(server_fd);
    return 0;
}
