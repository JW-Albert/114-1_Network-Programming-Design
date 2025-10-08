#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#define PORT 5678
#define BUF_SIZE 256

int main(){
    int sock;
    struct sockaddr_in serv;
    char user[BUF_SIZE], pw[BUF_SIZE], buf[BUF_SIZE];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr*)&serv, sizeof(serv));

    memset(buf, 0, sizeof(buf));
    recv(sock, buf, sizeof(buf) - 1, 0);
    printf("%s", buf);

    while (1) {
        printf("Username: ");
        fgets(user, sizeof(user), stdin);
        user[strcspn(user, "\n")] = 0;
        printf("Password: ");
        fflush(stdout);

        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        tv.tv_sec = 5; tv.tv_usec = 0;
        int ready = select(1, &fds, NULL, NULL, &tv);
        if (ready <= 0) {
            printf("\nTimeout! Login failed, wait 10s...\n");
            sleep(10);
            continue;
        }
        fgets(pw, sizeof(pw), stdin);
        pw[strcspn(pw, "\n")] = 0;

        char sendbuf[BUF_SIZE];
        snprintf(sendbuf, sizeof(sendbuf), "%s|%s", user, pw);
        send(sock, sendbuf, strlen(sendbuf), 0);

        memset(buf, 0, sizeof(buf));
        int len = recv(sock, buf, sizeof(buf) - 1, 0);
        if (len <= 0) break;
        printf("%s", buf);
        if (strstr(buf, "success")) break;
    }

    while (1) {
        memset(buf, 0, sizeof(buf));
        int len = recv(sock, buf, sizeof(buf) - 1, 0);
        if (len <= 0) break;
        printf("%s", buf);
        fgets(buf, sizeof(buf), stdin);
        send(sock, buf, strlen(buf), 0);
    }

    close(sock);
    return 0;
}
