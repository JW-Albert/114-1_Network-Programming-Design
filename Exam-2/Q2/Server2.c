#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 5678
#define MAX_ACC 3
#define BUF_SIZE 256

typedef struct {
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    time_t locked_until;
} Account;

Account accounts[MAX_ACC];
int acc_count = 0;

int find_account(char *user) {
    for (int i = 0; i < acc_count; i++)
        if (strcmp(accounts[i].username, user) == 0) return i;
    return -1;
}

int check_pw_format(char *pw) {
    int len = strlen(pw);
    if (len < 12 || len > 20) return 0;
    int upper = 0, lower = 0;
    for (int i = 0; i < len; i++) {
        if (pw[i] >= 'A' && pw[i] <= 'Z') upper = 1;
        if (pw[i] >= 'a' && pw[i] <= 'z') lower = 1;
    }
    return upper && lower;
}

void handle_client(int client_fd) {
    char buf[BUF_SIZE];
    char user[BUF_SIZE], pw[BUF_SIZE];
    send(client_fd, "Please login.\n", 14, 0);

    while (1) {
        memset(buf, 0, sizeof(buf));
        int len = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (len <= 0) break;
        sscanf(buf, "%[^|]|%s", user, pw);

        int idx = find_account(user);
        time_t now = time(NULL);
        if (idx >= 0 && now < accounts[idx].locked_until) {
            send(client_fd, "Account locked. Try again later.\n", 33, 0);
            continue;
        }

        if (idx >= 0 && strcmp(accounts[idx].password, pw) == 0) {
            send(client_fd, "Login success.\n", 15, 0);
            break;
        } else {
            if (idx >= 0) {
                accounts[idx].locked_until = now + 10;
            }
            send(client_fd, "Wrong password.\n", 16, 0);
        }
    }

    while (1) {
        char menu[] = "Select:\n1. Register new account\n2. Change password\n";
        send(client_fd, menu, strlen(menu), 0);
        memset(buf, 0, sizeof(buf));
        int len = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (len <= 0) break;

        if (buf[0] == '1') {
            if (acc_count >= MAX_ACC) {
                send(client_fd, "Full, cannot register.\n", 23, 0);
                continue;
            }
            char newu[BUF_SIZE], newp[BUF_SIZE];
            send(client_fd, "Enter new username:\n", 21, 0);
            recv(client_fd, newu, sizeof(newu) - 1, 0);
            newu[strcspn(newu, "\n")] = 0;
            if (find_account(newu) >= 0) {
                send(client_fd, "Username exists.\n", 17, 0);
                continue;
            }
            send(client_fd, "Enter new password:\n", 21, 0);
            recv(client_fd, newp, sizeof(newp) - 1, 0);
            newp[strcspn(newp, "\n")] = 0;
            if (!check_pw_format(newp)) {
                send(client_fd, "Please enter the new password again.\n", 37, 0);
                continue;
            }
            strcpy(accounts[acc_count].username, newu);
            strcpy(accounts[acc_count].password, newp);
            accounts[acc_count].locked_until = 0;
            acc_count++;
            send(client_fd, "Registered successfully.\n", 25, 0);
        } else if (buf[0] == '2') {
            char nowu[BUF_SIZE], newp[BUF_SIZE];
            send(client_fd, "Enter your username:\n", 22, 0);
            recv(client_fd, nowu, sizeof(nowu) - 1, 0);
            nowu[strcspn(nowu, "\n")] = 0;
            int idx = find_account(nowu);
            if (idx < 0) {
                send(client_fd, "User not found.\n", 16, 0);
                continue;
            }
            send(client_fd, "Enter new password:\n", 21, 0);
            recv(client_fd, newp, sizeof(newp) - 1, 0);
            newp[strcspn(newp, "\n")] = 0;
            if (!check_pw_format(newp)) {
                send(client_fd, "Please enter the new password again.\n", 37, 0);
                continue;
            }
            strcpy(accounts[idx].password, newp);
            send(client_fd, "Password updated successfully.\n", 32, 0);
        } else break;
    }
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in serv, cli;
    socklen_t addrlen = sizeof(cli);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr*)&serv, sizeof(serv));
    listen(server_fd, 3);
    printf("Server running on 127.0.0.1:%d ...\n", PORT);
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&cli, &addrlen);
        handle_client(client_fd);
    }
    close(server_fd);
    return 0;
}
