#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int main()
{
    int sock;
    struct sockaddr_in server;
    char recvbuf[BUFFER_SIZE];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5678);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&server, sizeof(server));
    printf("Connected to server.\n");
    while (1)
    {
        printf("\nSelect function:\n1. A\n2. B\n3. C\n4. Exit\nSelect: ");
        char opt[8];
        fgets(opt, sizeof(opt), stdin);
        opt[strcspn(opt, "\n")] = 0;
        if (opt[0] == '1')
        {
            int a = -1, b = -1;
            char operator= '+';
            char buf[BUFFER_SIZE];
            fd_set fds;
            struct timeval tv;
            printf("Enter first number (5s timeout): ");
            fflush(stdout);
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            int r = select(1, &fds, NULL, NULL, &tv);
            if (r > 0)
            {
                scanf("%d", &a);
                getchar();
            }
            else
            {
                printf("\nTimeout! A=-1\n");
            }
            printf("Enter second number (5s timeout): ");
            fflush(stdout);
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            r = select(1, &fds, NULL, NULL, &tv);
            if (r > 0)
            {
                scanf("%d", &b);
                getchar();
            }
            else
            {
                printf("\nTimeout! B=-1\n");
            }
            printf("Select operator (+, -, *, /): ");
            fflush(stdout);
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            r = select(1, &fds, NULL, NULL, &tv);
            if (r > 0)
            {
                scanf("%c", &operator);
                getchar();
            }
            else
            {
                printf("\nTimeout! Using default operator: +\n");
            }
            snprintf(buf, sizeof(buf), "A %d|%d|%c", a, b, operator);
            send(sock, buf, strlen(buf), 0);
            memset(recvbuf, 0, sizeof(recvbuf));
            recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
            printf("Server: %s\n", recvbuf);
        }
        else if (opt[0] == '2')
        {
            char buf[BUFFER_SIZE] = "B ";
            int nums[20];
            int count = 0;
            fd_set fds;
            struct timeval tv;
            printf("Enter numbers (min 4, max 10, 3s between each, timeout sets to 0):\n");
            while (count < 10)
            {
                printf("Input #%d: ", count + 1);
                fflush(stdout);
                FD_ZERO(&fds);
                FD_SET(0, &fds);
                tv.tv_sec = 3;
                tv.tv_usec = 0;
                int r = select(1, &fds, NULL, NULL, &tv);
                if (r <= 0)
                {
                    printf("\nTimeout! Setting to 0\n");
                    nums[count] = 0;
                    count++;
                    break;
                }
                scanf("%d", &nums[count]);
                getchar();
                count++;
            }
            for (int i = 0; i < count; i++)
            {
                char temp[32];
                snprintf(temp, sizeof(temp), "%d|", nums[i]);
                strcat(buf, temp);
            }
            send(sock, buf, strlen(buf), 0);
            memset(recvbuf, 0, sizeof(recvbuf));
            recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
            printf("Server: %s\n", recvbuf);
        }
        else if (opt[0] == '3')
        {
            char buf[BUFFER_SIZE] = "C ";
            char input[BUFFER_SIZE];
            fd_set fds;
            struct timeval tv;
            printf("Start typing letters (10s limit):\n");
            fflush(stdout);
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            tv.tv_sec = 10;
            tv.tv_usec = 0;
            int r = select(1, &fds, NULL, NULL, &tv);
            if (r > 0)
            {
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                strcat(buf, input);
            }
            else
            {
                printf("\nTimeout. Sending empty.\n");
            }
            send(sock, buf, strlen(buf), 0);
            memset(recvbuf, 0, sizeof(recvbuf));
            recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
            printf("Server: %s\n", recvbuf);
        }
        else if (opt[0] == '4')
        {
            char msg[] = "EXIT";
            send(sock, msg, strlen(msg), 0);
            break;
        }
    }
    close(sock);
    return 0;
}
