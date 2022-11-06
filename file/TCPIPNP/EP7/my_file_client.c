//
// Created by Administrator on 2022/11/6.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUF_SZ 1024

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int sock;
    char buff[BUF_SZ];
    int str_len;
    int file_fd;

    struct sockaddr_in adr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error!");

    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = inet_addr(argv[1]);
    adr.sin_port = htons(atoi(argv[2]));
    if (-1 == connect(sock, (const struct sockaddr *) &adr, sizeof(adr)))
        error_handling("connect() error!");

    file_fd = open("recv_file.c", O_CREAT | O_WRONLY);

    while (str_len = read(sock, buff, BUF_SZ)) {
        write(file_fd, buff, str_len);
    }

    strcpy(buff, "Thank you!");
    write(sock, buff, strlen(buff) + 1);

    close(file_fd);
    close(sock);
    return 0;
}


void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
