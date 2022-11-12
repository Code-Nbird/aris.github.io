//
// Created by Administrator on 2022/11/5.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in sock_addr;
    FILE * readfp;
    FILE * writefp;

    if (argc != 3) {
        fprintf(stdout, "Usage : %s <IP> <PORT> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error!");

    memset(&sock_addr,0,sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sock_addr.sin_port = htons(atoi(argv[2]));

    if (-1 == connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) )
        error_handling("connect() error!");
    else
        puts("Connected......");

    readfp = fdopen(sock,"r");
    writefp= fdopen(sock,"w");
    while(1) {
        fputs("Input message(Q to quit): ",stdout);
        fgets(message,BUF_SIZE,stdin);

        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n"))
            break;

        fputs(message,writefp);
        fflush(writefp);
        fgets(message,BUF_SIZE,readfp);
        fprintf(stdout,"Message from server : %s",message);
    }

    fclose(writefp);
    fclose(readfp);

    return 0;
}

void error_handling(char *message) {
    fputs( message, stderr);
    fputc('\n', stderr);
    exit(1);
}