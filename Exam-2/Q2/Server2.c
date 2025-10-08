#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 256
#define MAX_ACCOUNTS 3

typedef struct
{
    char studentID[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    time_t locked_until;
} Account;

Account accounts[MAX_ACCOUNTS];
int acc_count = 0;

int check_new_password_format(char *pw)
{
    int len = strlen(pw);
    if (len < 12 || len > 20)
        return 0;
    int upper = 0, lower = 0;
    for (int i = 0; i < len; i++)
    {
        if (pw[i] >= 'A' && pw[i] <= 'Z')
            upper = 1;
        if (pw[i] >= 'a' && pw[i] <= 'z')
            lower = 1;
    }
    return upper && lower;
}

int find_account(char *username)
{
    for (int i = 0; i < acc_count; i++)
        if (strcmp(accounts[i].username, username) == 0)
            return i;
    return -1;
}

void register_account(int client_fd, int send_login_prompt)
{
    char sid[BUFFER_SIZE], user[BUFFER_SIZE], pass[BUFFER_SIZE];
    memset(sid, 0, sizeof(sid));
    memset(user, 0, sizeof(user));
    memset(pass, 0, sizeof(pass));

    char msg1[] = "Enter Student ID: ";
    send(client_fd, msg1, strlen(msg1), 0);
    recv(client_fd, sid, sizeof(sid) - 1, 0);

    char msg2[] = "Enter Username: ";
    send(client_fd, msg2, strlen(msg2), 0);
    recv(client_fd, user, sizeof(user) - 1, 0);

    while (1)
    {
        char msg3[] = "Enter Password (6–15 chars): ";
        send(client_fd, msg3, strlen(msg3), 0);
        recv(client_fd, pass, sizeof(pass) - 1, 0);
        if (strlen(pass) < 6 || strlen(pass) > 15)
        {
            char msg4[] = "Invalid password length, please try again.\n";
            send(client_fd, msg4, strlen(msg4), 0);
            continue;
        }
        break;
    }

    if (acc_count >= MAX_ACCOUNTS)
    {
        char msg5[] = "Server full, cannot register new accounts.\n";
        send(client_fd, msg5, strlen(msg5), 0);
        return;
    }

    strcpy(accounts[acc_count].studentID, sid);
    strcpy(accounts[acc_count].username, user);
    strcpy(accounts[acc_count].password, pass);
    accounts[acc_count].locked_until = 0;
    acc_count++;

    char msg6[BUFFER_SIZE];
    snprintf(msg6, sizeof(msg6), "註冊成功: 學號=%s, 帳號=%s, 密碼=%s\n", sid, user, pass);
    send(client_fd, msg6, strlen(msg6), 0);
    printf("%s", msg6);

    if (send_login_prompt) {
        char msg7[] = "Please login.\n";
        send(client_fd, msg7, strlen(msg7), 0);
    }
}

void login_phase(int client_fd)
{
    char recvbuf[BUFFER_SIZE];
    char user[BUFFER_SIZE], pass[BUFFER_SIZE];
    time_t now;

    char msg8[] = "Please login.\n";
    send(client_fd, msg8, strlen(msg8), 0);

    while (1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
        sscanf(recvbuf, "%[^|]|%s", user, pass);
        int idx = find_account(user);
        now = time(NULL);

        if (idx < 0)
        {
            char msg9[] = "Wrong ID!!! Wait 5 seconds.\n";
            send(client_fd, msg9, strlen(msg9), 0);
            sleep(5);
            continue;
        }
        if (now < accounts[idx].locked_until)
        {
            char msg10[] = "Account locked. Try again later.\n";
            send(client_fd, msg10, strlen(msg10), 0);
            continue;
        }
        if (strcmp(accounts[idx].password, pass) != 0)
        {
            char msg11[] = "Wrong Password!!! Wait 5 seconds.\n";
            send(client_fd, msg11, strlen(msg11), 0);
            accounts[idx].locked_until = now + 10;
            sleep(5);
            continue;
        }

        char msg12[] = "Login success!\n";
        send(client_fd, msg12, strlen(msg12), 0);
        printf("Client logged in successfully: %s\n", user);
        break;
    }
}

void handle_options(int client_fd)
{
    char recvbuf[BUFFER_SIZE];
    while (1)
    {
        char msg13[] = "\nSelect option:\n1. Register new account\n2. Change password\n3. Exit\n";
        send(client_fd, msg13, strlen(msg13), 0);
        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(client_fd, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0)
            break;

        if (recvbuf[0] == '1')
        {
            register_account(client_fd, 0);
        }
        else if (recvbuf[0] == '2')
        {
            char user[BUFFER_SIZE], newpw[BUFFER_SIZE];
            char msg14[] = "Enter your username: ";
            send(client_fd, msg14, strlen(msg14), 0);
            memset(user, 0, sizeof(user));
            recv(client_fd, user, sizeof(user) - 1, 0);
            int idx = find_account(user);
            if (idx < 0)
            {
                char msg15[] = "User not found.\n";
                send(client_fd, msg15, strlen(msg15), 0);
                continue;
            }

            while (1)
            {
                char msg16[] = "Enter new password (12–20 chars, upper & lower required): ";
                send(client_fd, msg16, strlen(msg16), 0);
                memset(newpw, 0, sizeof(newpw));
                recv(client_fd, newpw, sizeof(newpw) - 1, 0);
                if (!check_new_password_format(newpw))
                {
                    char msg17[] = "Please enter the new password again.\n";
                    send(client_fd, msg17, strlen(msg17), 0);
                    continue;
                }
                break;
            }
            strcpy(accounts[idx].password, newpw);
            char msg18[] = "Password changed successfully.\n";
            send(client_fd, msg18, strlen(msg18), 0);
        }
        else if (recvbuf[0] == '3')
            break;
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5678);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
        register_account(client_fd, 1);
        login_phase(client_fd);
        handle_options(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
