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

void trim(char *s){
    int n = strlen(s);
    while(n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' '))
        s[--n] = 0;
}

int find_idx(const char* u){
    for(int i = 0; i < acc_count; i++)
        if(strcmp(accounts[i].username, u) == 0)
            return i;
    return -1;
}

int check_new_pw(const char* p){
    int n = strlen(p);
    if(n < 12 || n > 20)
        return 0;
    int U = 0, L = 0;
    for(int i = 0; i < n; i++){
        if(p[i] >= 'A' && p[i] <= 'Z')
            U = 1;
        if(p[i] >= 'a' && p[i] <= 'z')
            L = 1;
    }
    return U && L;
}

void register_account(int cfd, int send_login_prompt){
    char sid[BUFFER_SIZE] = {0};
    char user[BUFFER_SIZE] = {0};
    char pass[BUFFER_SIZE] = {0};

    char msg1[] = "Enter Student ID: ";
    send(cfd, msg1, strlen(msg1), 0);
    recv(cfd, sid, sizeof(sid) - 1, 0);
    trim(sid);

    char msg2[] = "Enter Username: ";
    send(cfd, msg2, strlen(msg2), 0);
    recv(cfd, user, sizeof(user) - 1, 0);
    trim(user);

    char msg3[] = "Enter Password (6-15 chars): ";
    send(cfd, msg3, strlen(msg3), 0);
    recv(cfd, pass, sizeof(pass) - 1, 0);
    trim(pass);

    if(acc_count >= MAX_ACCOUNTS){
        char msgfull[] = "Server full, cannot register new accounts.\n";
        send(cfd, msgfull, strlen(msgfull), 0);
        return;
    }

    strcpy(accounts[acc_count].studentID, sid);
    strcpy(accounts[acc_count].username, user);
    strcpy(accounts[acc_count].password, pass);
    accounts[acc_count].locked_until = 0;
    acc_count++;

    char ok[BUFFER_SIZE];
    snprintf(ok, sizeof(ok), "註冊成功: 學號=%s, 帳號=%s, 密碼=%s\n", sid, user, pass);
    send(cfd, ok, strlen(ok), 0);
    printf("%s", ok);

    if(send_login_prompt){
        char msglogin[] = "Please login\n";
        send(cfd, msglogin, strlen(msglogin), 0);
    }
}

void login_phase(int cfd){
    char msghi[] = "Please login\n";
    send(cfd, msghi, strlen(msghi), 0);

    char buf[BUFFER_SIZE] = {0};
    char u[BUFFER_SIZE] = {0};
    char p[BUFFER_SIZE] = {0};

    while(1){
        memset(buf, 0, sizeof(buf));
        int n = recv(cfd, buf, sizeof(buf) - 1, 0);
        if(n <= 0)
            return;

        char* t = strtok(buf, "|");
        if(t)
            strcpy(u, t);

        t = strtok(NULL, "|");
        if(t)
            strcpy(p, t);

        trim(u);
        trim(p);

        int idx = find_idx(u);
        time_t now = time(NULL);

        if(idx < 0){
            char m[] = "Wrong ID!!! Wait 5 seconds.\n";
            send(cfd, m, strlen(m), 0);
            sleep(5);
            continue;
        }

        if(now < accounts[idx].locked_until){
            char m[] = "Account locked. Try again later.\n";
            send(cfd, m, strlen(m), 0);
            continue;
        }

        if(strcmp(p, accounts[idx].password) != 0){
            char m[] = "Wrong Password!!! Wait 5 seconds.\n";
            send(cfd, m, strlen(m), 0);
            accounts[idx].locked_until = now + 10;
            sleep(5);
            continue;
        }

        char ok[] = "Login success!\n";
        send(cfd, ok, strlen(ok), 0);
        printf("Client logged in successfully: %s\n", u);
        break;
    }
}

void handle_options(int cfd){
    char buf[BUFFER_SIZE];

    while(1){
        char menu[] = "\nSelect option:\n1. Register new account\n2. Change password\n3. Exit\n";
        send(cfd, menu, strlen(menu), 0);

        memset(buf, 0, sizeof(buf));
        int n = recv(cfd, buf, sizeof(buf) - 1, 0);
        if(n <= 0)
            break;

        if(buf[0] == '1'){
            register_account(cfd, 0);
        }
        else if(buf[0] == '2'){
            char u[BUFFER_SIZE] = {0};
            char npw[BUFFER_SIZE] = {0};

            char q1[] = "Enter your username: ";
            send(cfd, q1, strlen(q1), 0);
            recv(cfd, u, sizeof(u) - 1, 0);
            trim(u);

            int idx = find_idx(u);
            if(idx < 0){
                char m[] = "User not found.\n";
                send(cfd, m, strlen(m), 0);
                continue;
            }

            while(1){
                char q2[] = "Enter new password (12-20, need upper & lower): ";
                send(cfd, q2, strlen(q2), 0);
                memset(npw, 0, sizeof(npw));
                recv(cfd, npw, sizeof(npw) - 1, 0);
                trim(npw);

                if(!check_new_pw(npw)){
                    char bad[] = "Please enter the new password again.\n";
                    send(cfd, bad, strlen(bad), 0);
                    continue;
                }
                break;
            }

            strcpy(accounts[idx].password, npw);
            char ok[] = "Password changed successfully.\n";
            send(cfd, ok, strlen(ok), 0);
        }
        else if(buf[0] == '3'){
            break;
        }
    }
}

int main(){
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int cfd;
    struct sockaddr_in sa, ca;
    socklen_t alen = sizeof(ca);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(5678);
    sa.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (struct sockaddr*)&sa, sizeof(sa));
    listen(sfd, 1);
    printf("Server started on 127.0.0.1:5678 ...\n");

    while(1){
        cfd = accept(sfd, (struct sockaddr*)&ca, &alen);
        printf("Client connected: %s\n", inet_ntoa(ca.sin_addr));
        register_account(cfd, 1);
        login_phase(cfd);
        handle_options(cfd);
        close(cfd);
    }

    close(sfd);
    return 0;
}
