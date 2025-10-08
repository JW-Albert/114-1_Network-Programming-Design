#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server;
    char input[BUFFER_SIZE], sendbuf[BUFFER_SIZE], recvbuf[BUFFER_SIZE];
    char login_user[BUFFER_SIZE], login_pass[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5678);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr*)&server, sizeof(server));
    printf("Connected to server.\n");

    for (int i = 0; i < 3; i++) {
        memset(recvbuf, 0, sizeof(recvbuf));
        recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        printf("%s", recvbuf);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        send(sock, input, strlen(input), 0);
    }

    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0) break;
        printf("%s", recvbuf);
        if (strstr(recvbuf, "註冊成功")) break;
    }

    memset(recvbuf, 0, sizeof(recvbuf));
    recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
    printf("%s", recvbuf);

    while (1) {
        printf("Login Username: ");
        fgets(login_user, sizeof(login_user), stdin);
        login_user[strcspn(login_user, "\n")] = 0;

        printf("Login Password (5s timeout): ");
        fflush(stdout);

        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        tv.tv_sec = 5; tv.tv_usec = 0;

        int ready = select(1, &fds, NULL, NULL, &tv);
        if (ready <= 0) {
            printf("\nTimeout! Please wait 10 seconds before retry.\n");
            sleep(10);
            continue;
        }

        fgets(login_pass, sizeof(login_pass), stdin);
        login_pass[strcspn(login_pass, "\n")] = 0;

        snprintf(sendbuf, sizeof(sendbuf), "%s|%s", login_user, login_pass);
        send(sock, sendbuf, strlen(sendbuf), 0);

        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0) break;
        printf("%s", recvbuf);
        if (strstr(recvbuf, "success")) break;
    }

    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        int len = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (len <= 0) break;
        printf("%s", recvbuf);
        fgets(sendbuf, sizeof(sendbuf), stdin);
        send(sock, sendbuf, strlen(sendbuf), 0);
    }

    close(sock);
    return 0;
}
