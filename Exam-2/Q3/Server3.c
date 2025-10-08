#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main() {
    int sock, csock, addrlen;
    struct sockaddr_in server, client;
    char buf[BUFFER_SIZE], msg[BUFFER_SIZE];
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(5678);
    server.sin_addr.s_addr=INADDR_ANY;
    sock=socket(AF_INET,SOCK_STREAM,0);
    bind(sock,(struct sockaddr*)&server,sizeof(server));
    listen(sock,5);
    addrlen=sizeof(client);
    csock=accept(sock,(struct sockaddr*)&client,&addrlen);
    while(1){
        memset(buf,0,sizeof(buf));
        int len=recv(csock,buf,sizeof(buf),0);
        if(len<=0) break;
        buf[len]=0;
        if(strcmp(buf,"A")==0){
            char a_str[BUFFER_SIZE], b_str[BUFFER_SIZE], op[BUFFER_SIZE];
            recv(csock,a_str,sizeof(a_str),0);
            recv(csock,b_str,sizeof(b_str),0);
            recv(csock,op,sizeof(op),0);
            int a=atoi(a_str), b=atoi(b_str);
            if(a==-1||b==-1){ send(csock,"Request Denied\n",15,0); continue; }
            double res=0;
            if(strcmp(op,"+")==0) res=a+b;
            else if(strcmp(op,"-")==0) res=a-b;
            else if(strcmp(op,"*")==0) res=a*b;
            else if(strcmp(op,"/")==0) res=b!=0?(double)a/b:0;
            sprintf(msg,"Result: %.2f\n",res);
            send(csock,msg,strlen(msg),0);
        } else if(strcmp(buf,"B")==0){
            int nums[10], count=0, zeros=0;
            while(1){
                recv(csock,msg,sizeof(msg),0);
                if(strcmp(msg,"END")==0) break;
                int n=atoi(msg);
                nums[count++]=n;
                if(n==0) zeros++;
            }
            if(zeros>=2){ send(csock,"No valid data\n",14,0); continue; }
            for(int i=0;i<count-1;i++)
                for(int j=i+1;j<count;j++)
                    if(nums[i]>nums[j]){int t=nums[i];nums[i]=nums[j];nums[j]=t;}
            char out[BUFFER_SIZE]="";
            for(int i=0;i<count;i++){ char tmp[32]; sprintf(tmp,"%d ",nums[i]); strcat(out,tmp); }
            strcat(out,"\n");
            send(csock,out,strlen(out),0);
        } else if(strcmp(buf,"C")==0){
            char data[BUFFER_SIZE]; recv(csock,data,sizeof(data),0);
            int freq[256]={0};
            for(int i=0;data[i];i++) if(isalpha(data[i])) freq[(unsigned char)data[i]]++;
            char out[BUFFER_SIZE]="";
            for(int i='A';i<='z';i++){
                if(freq[i]>0){ char tmp[32]; sprintf(tmp,"%c:%d ",i,freq[i]); strcat(out,tmp); }
            }
            strcat(out,"\n");
            send(csock,out,strlen(out),0);
        }
    }
    close(csock);
    close(sock);
    return 0;
}
