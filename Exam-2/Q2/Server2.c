#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 256
#define MAX_ACCOUNTS 3

typedef struct {
    char studentID[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    time_t locked_until;
} Account;

Account accounts[MAX_ACCOUNTS];
int acc_count = 0;

int check_new_password_format(char *pw) {
    int len = strlen(pw);
    if (len < 12 || len > 20) return 0;
    int upper = 0, lower = 0;
    for (int i = 0; i < len; i++) {
        if (pw[i] >= 'A' && pw[i] <= 'Z') upper = 1;
        if (pw[i] >= 'a' && pw[i] <= 'z') lower = 1;
    }
    return upper && lower;
}

int find_account(char *username) {
    for (int i = 0; i < acc_count; i++)
        if (strcmp(accounts[i].username, username) == 0)
            return i;
    return -1;
}

void register_account(int client_fd, int send_login_prompt) {
    char sid[BUFFER_SIZE], user[BUFFER_SIZE], pass[BUFFER_SIZE];
    memset(sid, 0, sizeof(sid));
    memset(user, 0, sizeof(user));
    memset(pass, 0, sizeof(pass));

    send(client_fd, "Enter Student ID: ", 19, 0);
    recv(client_fd, sid, sizeof(sid) - 1, 0);

    send(client_fd, "Enter Username: ", 17, 0);
    recv(client_fd, user, sizeof(user) - 1, 0);

    while (1) {
        send(client_fd, "Enter Password (6–15 chars): ", 30, 0);
        recv(client_fd, pass, sizeof(pass) - 1, 0);
        if (strlen(pass) < 6 || strlen(pass) > 15) {
            send(client_fd, "Invalid password length, please try again.\n", 43, 0);
            continue;
        }
        break;
    }

    if (acc_count >= MAX_ACCOUNTS) {
        send(client_fd, "Server full, cannot register new accounts.\n", 43, 0);
        return;
    }

    strcpy(accounts[acc_count].studentID, sid);
    strcpy(accounts[acc_count].username, user);
    strcpy(accounts[acc_count].password, pass);
    accounts[acc_count].locked_until = 0;
    acc_count++;

    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof(msg), "註冊成功: 學號=%s, 帳號=%s, 密碼=%s\n", sid, user, pass);
    send(client_fd, msg, strlen(msg), 0);
    printf("%s", msg);

    if (send_login_prompt)
        send(client_fd, "Please login.\n", 14, 0);
}


void login_phase(int client_fd) {
    char recvbuf[BUFFER_SIZE];
    char user[BUFFER_SIZE], pass[BUFFER_SIZE];
    time_t now;

    send(client_fd, "Please login.\n", 14, 0);

    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
        sscanf(recvbuf, "%[^|]|%s", user, pass);
        int idx = find_account(user);
        now = time(NULL);

        if (idx < 0) {
            send(client_fd, "Wrong ID!!! Wait 5 seconds.\n", 28, 0);
            sleep(5);
            continue;
        }
        if (now < accounts[idx].locked_until) {
            send(client_fd, "Account locked. Try again later.\n", 33, 0);
            continue;
        }
        if (strcmp(accounts[idx].password, pass) != 0) {
            send(client_fd, "Wrong Password!!! Wait 5 seconds.\n", 34, 0);
            accounts[idx].locked_until = now + 10;
            sleep(5);
            continue;
        }

        send(client_fd, "Login success!\n", 15, 0);
        printf("Client logged in successfully: %s\n", user);
        break;
    }
}

void handle_options(int client_fd) {
    char recvbuf[BUFFER_SIZE];
    while (1) {
        send(client_fd, "\nSelect option:\n1. Register new account\n2. Change password\n3. Exit\n", 70, 0);
        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0) break;

        if (recvbuf[0] == '1') {
            register_account(client_fd, 0);
        } else if (recvbuf[0] == '2') {
            char user[BUFFER_SIZE], newpw[BUFFER_SIZE];
            send(client_fd, "Enter your username: ", 22, 0);
            memset(user, 0, sizeof(user));
            recv(client_fd, user, sizeof(user) - 1, 0);
            int idx = find_account(user);
            if (idx < 0) {
                send(client_fd, "User not found.\n", 16, 0);
                continue;
            }

            while (1) {
                send(client_fd, "Enter new password (12–20 chars, upper & lower required): ", 61, 0);
                memset(newpw, 0, sizeof(newpw));
                recv(client_fd, newpw, sizeof(newpw) - 1, 0);
                if (!check_new_password_format(newpw)) {
                    send(client_fd, "Invalid password format, please try again.\n", 43, 0);
                    send(client_fd, "Please enter the new password again.\n", 37, 0);
                    continue;
                }
                break;
            }
            strcpy(accounts[idx].password, newpw);
            send(client_fd, "Password changed successfully.\n", 32, 0);
        } else if (recvbuf[0] == '3') break;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5678);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
        register_account(client_fd, 1);
        login_phase(client_fd);
        handle_options(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
