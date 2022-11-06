//
// Created by Administrator on 2022/11/5.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 30

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t addr_sz;
    struct sockaddr_in serv_addr, from_addr;

    if (argc != 3) {
        fprintf(stdout, "Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handling("socket() error!");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n"))
            break;

        sendto(sock, message, strlen(message), 0,
               (const struct sockaddr *) &serv_addr,
               sizeof(serv_addr));

        str_len = recvfrom(sock, message, BUF_SIZE, 0,
                 (struct sockaddr *) &from_addr,
                 &addr_sz);
        message[str_len] = 0;
        fprintf(stdout, "Message from server : %s", message);
    }
    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}