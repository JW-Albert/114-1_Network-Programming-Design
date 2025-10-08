#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int has_upper(const char *s)
{
    for (int i = 0; s[i]; i++)
        if (s[i] >= 'A' && s[i] <= 'Z')
            return 1;
    return 0;
}

int has_lower(const char *s)
{
    for (int i = 0; s[i]; i++)
        if (s[i] >= 'a' && s[i] <= 'z')
            return 1;
    return 0;
}

int main()
{
    int sock;
    struct sockaddr_in server;
    char recvbuf[BUFFER_SIZE];
    char sid[BUFFER_SIZE];
    char user[BUFFER_SIZE];
    char pw[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5678);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&server, sizeof(server));
    printf("Connected to server.\n");

    // === 註冊階段 ===
    printf("=== Register New Account ===\n");
    printf("Enter Student ID: ");
    fgets(sid, sizeof(sid), stdin);
    sid[strcspn(sid, "\n")] = 0;

    printf("Enter Username: ");
    fgets(user, sizeof(user), stdin);
    user[strcspn(user, "\n")] = 0;

    while (1)
    {
        printf("Enter Password (6-15 chars): ");
        fgets(pw, sizeof(pw), stdin);
        pw[strcspn(pw, "\n")] = 0;
        if (strlen(pw) >= 6 && strlen(pw) <= 15)
            break;
        printf("Invalid length. Please try again.\n");
    }

    char regbuf[BUFFER_SIZE];
    snprintf(regbuf, sizeof(regbuf), "REGISTER %s|%s|%s", sid, user, pw);
    send(sock, regbuf, strlen(regbuf), 0);
    memset(recvbuf, 0, sizeof(recvbuf));
    recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);

    if (strstr(recvbuf, "REGISTER_OK"))
        printf("註冊成功！\n");
    else if (strstr(recvbuf, "SERVER_FULL"))
    {
        printf("Server full. Exit.\n");
        close(sock);
        return 0;
    }
    else if (strstr(recvbuf, "STUDENT_ID_EXISTS"))
    {
        printf("學號已存在，註冊失敗。\n");
        close(sock);
        return 0;
    }
    else if (strstr(recvbuf, "USERNAME_EXISTS"))
    {
        printf("用戶名已存在，註冊失敗。\n");
        close(sock);
        return 0;
    }
    else
    {
        printf("註冊失敗。\n");
        close(sock);
        return 0;
    }

    // === 登入階段 ===
    while (1)
    {
        printf("\n=== Login ===\n");
        printf("Username: ");
        fgets(user, sizeof(user), stdin);
        user[strcspn(user, "\n")] = 0;

        printf("Password (5s timeout): ");
        fflush(stdout);

        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int r = select(1, &fds, NULL, NULL, &tv);
        if (r <= 0)
        {
            printf("\nTimeout! Wait 10 seconds.\n");
            sleep(10);
            continue;
        }

        fgets(pw, sizeof(pw), stdin);
        pw[strcspn(pw, "\n")] = 0;

        char logbuf[BUFFER_SIZE];
        snprintf(logbuf, sizeof(logbuf), "LOGIN %s|%s", user, pw);
        send(sock, logbuf, strlen(logbuf), 0);

        memset(recvbuf, 0, sizeof(recvbuf));
        recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);

        if (strstr(recvbuf, "WRONG_ID"))
        {
            printf("Wrong ID. Wait 5 seconds.\n");
            sleep(10);
        }
        else if (strstr(recvbuf, "LOCKED"))
        {
            printf("Account locked. Try later.\n");
            sleep(10);
        }
        else if (strstr(recvbuf, "WRONG_PW"))
        {
            printf("Wrong password. Wait 5 seconds.\n");
            sleep(10);
        }
        else if (strstr(recvbuf, "LOGIN_OK"))
        {
            printf("Login success!\n");
            break;
        }
    }

    // === 主選單 ===
    while (1)
    {
        printf("\n1. Register new account\n2. Change password\n3. Exit\n");
        printf("Select: ");
        char opt[8];
        fgets(opt, sizeof(opt), stdin);
        opt[strcspn(opt, "\n")] = 0;

        if (opt[0] == '1')
        {
            printf("Enter Student ID: ");
            fgets(sid, sizeof(sid), stdin);
            sid[strcspn(sid, "\n")] = 0;
            printf("Enter Username: ");
            fgets(user, sizeof(user), stdin);
            user[strcspn(user, "\n")] = 0;
            while (1)
            {
                printf("Enter Password (6-15): ");
                fgets(pw, sizeof(pw), stdin);
                pw[strcspn(pw, "\n")] = 0;
                if (strlen(pw) >= 6 && strlen(pw) <= 15)
                    break;
                printf("Invalid length.\n");
            }
            char buf[BUFFER_SIZE];
            snprintf(buf, sizeof(buf), "REGISTER %s|%s|%s", sid, user, pw);
            send(sock, buf, strlen(buf), 0);
            recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);

            if (strstr(recvbuf, "REGISTER_OK"))
                printf("註冊成功！\n");
            else if (strstr(recvbuf, "SERVER_FULL"))
                printf("Server full.\n");
            else if (strstr(recvbuf, "STUDENT_ID_EXISTS"))
                printf("學號已存在，註冊失敗。\n");
            else if (strstr(recvbuf, "USERNAME_EXISTS"))
                printf("用戶名已存在，註冊失敗。\n");
            else
                printf("註冊失敗。\n");
        }
        else if (opt[0] == '2')
        {
            char newpw[BUFFER_SIZE];
            while (1)
            {
                printf("Enter new password (12-20, upper & lower): ");
                fgets(newpw, sizeof(newpw), stdin);
                newpw[strcspn(newpw, "\n")] = 0;
                if (strlen(newpw) >= 12 && strlen(newpw) <= 20 && has_upper(newpw) && has_lower(newpw))
                    break;
                printf("Invalid format.\n");
            }
            char buf[BUFFER_SIZE];
            snprintf(buf, sizeof(buf), "CHPASS %s|%s", user, newpw);
            send(sock, buf, strlen(buf), 0);
            recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
            printf("%s\n", recvbuf);
        }
        else if (opt[0] == '3')
        {
            char msg[] = "EXIT";
            send(sock, msg, strlen(msg), 0);
            break;
        }
    }

    close(sock);
    return 0;
}
