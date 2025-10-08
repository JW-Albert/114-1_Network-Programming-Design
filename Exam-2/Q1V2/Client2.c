#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server;
    char studentID[BUFFER_SIZE], username[BUFFER_SIZE], password[BUFFER_SIZE];
    char login_user[BUFFER_SIZE], login_pass[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE], recvbuf[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5678);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr*)&server, sizeof(server));

    printf("Connected to server.\n");

    printf("Enter Student ID: ");
    fgets(studentID, sizeof(studentID), stdin);
    studentID[strcspn(studentID, "\n")] = 0;
    send(sock, studentID, sizeof(studentID), 0);

    printf("Create Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    send(sock, username, sizeof(username), 0);

    do {
        printf("Create Password (6-15 chars): ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = 0;
    } while (strlen(password) < 6 || strlen(password) > 15);
    send(sock, password, sizeof(password), 0);
    printf("Registration complete.\n");

    recv(sock, recvbuf, sizeof(recvbuf), 0);
    printf("%s", recvbuf);

    printf("Login Username: ");
    fgets(login_user, sizeof(login_user), stdin);
    login_user[strcspn(login_user, "\n")] = 0;
    send(sock, login_user, sizeof(login_user), 0);

    printf("Login Password: ");
    fgets(login_pass, sizeof(login_pass), stdin);
    login_pass[strcspn(login_pass, "\n")] = 0;
    send(sock, login_pass, sizeof(login_pass), 0);

    recv(sock, recvbuf, sizeof(recvbuf), 0);
    printf("%s", recvbuf);

    if (strstr(recvbuf, "success") != NULL) {
        fd_set fds;
        while (1) {
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            FD_SET(sock, &fds);
            int maxfd = sock + 1;
            select(maxfd, &fds, NULL, NULL, NULL);

            if (FD_ISSET(sock, &fds)) {
                int len = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
                if (len <= 0) break;
                recvbuf[len] = '\0';
                if (strcmp(recvbuf, "exit") == 0) break;
                printf("Server: %s\n", recvbuf);
            }

            if (FD_ISSET(0, &fds)) {
                fgets(sendbuf, sizeof(sendbuf), stdin);
                sendbuf[strcspn(sendbuf, "\n")] = 0;
                send(sock, sendbuf, strlen(sendbuf), 0);
                if (strcmp(sendbuf, "exit") == 0) break;
            }
        }
    }

    close(sock);
    return 0;
}
