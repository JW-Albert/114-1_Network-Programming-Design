#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 256
#define MAX_ACCOUNTS 10

typedef struct
{
    char studentID[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    time_t locked_until;
} Account;

Account accounts[MAX_ACCOUNTS];
int acc_count = 0;

int find_idx(const char *u)
{
    for (int i = 0; i < acc_count; i++)
    {
        if (strcmp(accounts[i].username, u) == 0)
            return i;
    }
    return -1;
}

void register_account(int cfd, char *cmd)
{
    char sid[BUFFER_SIZE], user[BUFFER_SIZE], pw[BUFFER_SIZE];
    // 跳過 "REGISTER " 前綴，直接解析學號|用戶名|密碼
    char *data = cmd;
    if (strncmp(cmd, "REGISTER ", 9) == 0)
    {
        data = cmd + 9; // 跳過 "REGISTER "
    }
    sscanf(data, "%[^|]|%[^|]|%s", sid, user, pw);

    if (acc_count >= MAX_ACCOUNTS)
    {
        char msg[] = "SERVER_FULL";
        send(cfd, msg, strlen(msg), 0);
        return;
    }

    strcpy(accounts[acc_count].studentID, sid);
    strcpy(accounts[acc_count].username, user);
    strcpy(accounts[acc_count].password, pw);
    accounts[acc_count].locked_until = 0;
    acc_count++;

    char msg[] = "REGISTER_OK";
    send(cfd, msg, strlen(msg), 0);
    printf("註冊成功: %s %s %s\n", sid, user, pw);
}

void login_phase(int cfd, char *cmd)
{
    char buf[BUFFER_SIZE];
    char u[BUFFER_SIZE];
    char p[BUFFER_SIZE];

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        int n = recv(cfd, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            return;

        // 跳過 "LOGIN " 前綴
        char *data = buf;
        if (strncmp(buf, "LOGIN ", 6) == 0)
        {
            data = buf + 6; // 跳過 "LOGIN "
        }
        sscanf(data, "%[^|]|%s", u, p);

        int idx = find_idx(u);
        time_t now = time(NULL);

        if (idx < 0)
        {
            char msg[] = "WRONG_ID";
            send(cfd, msg, strlen(msg), 0);
            continue;
        }

        if (now < accounts[idx].locked_until)
        {
            char msg[] = "LOCKED";
            send(cfd, msg, strlen(msg), 0);
            continue;
        }

        if (strcmp(p, accounts[idx].password) != 0)
        {
            char msg[] = "WRONG_PW";
            send(cfd, msg, strlen(msg), 0);
            accounts[idx].locked_until = now + 10;
            continue;
        }

        char msg[] = "LOGIN_OK";
        send(cfd, msg, strlen(msg), 0);
        printf("登入成功: %s\n", u);
        break;
    }
}

void change_password(int cfd)
{
    char buf[BUFFER_SIZE] = {0};
    recv(cfd, buf, sizeof(buf) - 1, 0);

    char user[BUFFER_SIZE], newpw[BUFFER_SIZE];
    // 跳過 "CHPASS " 前綴
    char *data = buf;
    if (strncmp(buf, "CHPASS ", 7) == 0)
    {
        data = buf + 7; // 跳過 "CHPASS "
    }
    sscanf(data, "%[^|]|%s", user, newpw);

    int idx = find_idx(user);
    if (idx < 0)
    {
        char msg[] = "USER_NOT_FOUND";
        send(cfd, msg, strlen(msg), 0);
        return;
    }

    strcpy(accounts[idx].password, newpw);
    char msg[] = "PW_CHANGED";
    send(cfd, msg, strlen(msg), 0);
    printf("使用者 %s 修改密碼成功\n", user);
}

int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa, ca;
    socklen_t alen = sizeof(ca);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(5678);
    sa.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (struct sockaddr *)&sa, sizeof(sa));
    listen(sfd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    while (1)
    {
        int cfd = accept(sfd, (struct sockaddr *)&ca, &alen);
        printf("Client connected: %s\n", inet_ntoa(ca.sin_addr));

        while (1)
        {
            char cmd[BUFFER_SIZE] = {0};
            int n = recv(cfd, cmd, sizeof(cmd) - 1, 0);
            if (n <= 0)
                break;

            if (strncmp(cmd, "REGISTER", 8) == 0)
            {
                register_account(cfd, cmd);
            }
            else if (strncmp(cmd, "LOGIN", 5) == 0)
            {
                login_phase(cfd);
            }
            else if (strncmp(cmd, "CHPASS", 6) == 0)
            {
                change_password(cfd);
            }
            else if (strncmp(cmd, "EXIT", 4) == 0)
            {
                break;
            }
        }
        close(cfd);
    }

    close(sfd);
    return 0;
}
