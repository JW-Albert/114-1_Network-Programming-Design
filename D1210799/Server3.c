#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 256

void handle_A(int cfd, char *buf) {
    int a, b;
    char operator;
    sscanf(buf, "%d|%d|%c", &a, &b, &operator);
    if (a == -1 || b == -1) {
        char msg[] = "Request Denied";
        send(cfd, msg, strlen(msg), 0);
        return;
    }
    int result;
    switch (operator) {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
            result = a * b;
            break;
        case '/':
            if (b == 0)
            {
                char msg[] = "Division by zero";
                send(cfd, msg, strlen(msg), 0);
                return;
            }
            result = a / b;
            break;
        default:
            char msg[] = "Invalid operator";
            send(cfd, msg, strlen(msg), 0);
            return;
    }
    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof(msg), "Result: %d", result);
    send(cfd, msg, strlen(msg), 0);
}

void handle_B(int cfd, char *buf) {
    int nums[20];
    int count = 0;
    int zero_count = 0;
    char *token = strtok(buf, "|");
    while (token && count < 20) {
        nums[count] = atoi(token);
        if (nums[count] == 0) {
            zero_count++;
        }
        count++;
        token = strtok(NULL, "|");
    }
    if (count < 4) {
        char msg[] = "No valid data";
        send(cfd, msg, strlen(msg), 0);
        return;
    }
    if (zero_count >= 2) {
        char msg[] = "No valid data";
        send(cfd, msg, strlen(msg), 0);
        return;
    }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (nums[i] > nums[j]) {
                int t = nums[i];
                nums[i] = nums[j];
                nums[j] = t;
            }
        }
    }
    char msg[BUFFER_SIZE] = "";
    strcat(msg, "Sorted: ");
    for (int i = 0; i < count; i++) {
        char temp[32];
        snprintf(temp, sizeof(temp), "%d ", nums[i]);
        strcat(msg, temp);
    }
    send(cfd, msg, strlen(msg), 0);
}

void handle_C(int cfd, char *buf) {
    int count[256] = {0};
    for (int i = 0; buf[i]; i++) {
        if (isalpha(buf[i]))
            count[(unsigned char)buf[i]]++;
    }
    char msg[BUFFER_SIZE] = "";
    strcat(msg, "Statistics:\n");
    for (int i = 0; i < 256; i++) {
        if (count[i] > 0) {
            char temp[64];
            snprintf(temp, sizeof(temp), "%c: %d\n", i, count[i]);
            strcat(msg, temp);
        }
    }
    send(cfd, msg, strlen(msg), 0);
}

int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa, ca;
    socklen_t alen = sizeof(ca);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(5678);
    sa.sin_addr.s_addr = INADDR_ANY;
    bind(sfd, (struct sockaddr *)&sa, sizeof(sa));
    listen(sfd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");
    while (1) {
        int cfd = accept(sfd, (struct sockaddr *)&ca, &alen);
        printf("Client connected: %s\n", inet_ntoa(ca.sin_addr));
        while (1) {
            char buf[BUFFER_SIZE] = {0};
            int n = recv(cfd, buf, sizeof(buf) - 1, 0);
            if (n <= 0)
                break;
            if (strncmp(buf, "A ", 2) == 0) {
                handle_A(cfd, buf + 2);
            } else if (strncmp(buf, "B ", 2) == 0) {
                handle_B(cfd, buf + 2);
            } else if (strncmp(buf, "C ", 2) == 0) {
                handle_C(cfd, buf + 2);
            } else if (strncmp(buf, "EXIT", 4) == 0) {
                break;
            }
        }
        close(cfd);
    }
    close(sfd);
    return 0;
}
