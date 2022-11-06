//
// Created by Administrator on 2022/11/6.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    char buff[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int file_fd;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sock, (const struct sockaddr *) &serv_adr, sizeof(serv_adr)))
        error_handling("bind() error!");

    if (-1 == listen(serv_sock, 5))
        error_handling("listen() error");

    clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &clnt_adr_sz);
    if (clnt_sock == -1)
        error_handling("accept() error!");

    file_fd = open("file_server.c", O_RDONLY);

    while (str_len = read(file_fd, buff, BUF_SIZE)) {
        write(clnt_sock, buff, str_len);
    }

    shutdown(clnt_sock, O_WRONLY);
    str_len = read(clnt_sock, buff, BUF_SIZE);
    buff[str_len] = 0;
    printf("Receive client msg : %s\n", buff);
    close(file_fd);
    close(clnt_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
