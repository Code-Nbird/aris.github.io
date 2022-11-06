//
// Created by Administrator on 2022/11/6.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 30

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int serv_sd, clnt_sd;
    char buf[BUF_SIZE];
    FILE *fp;
    int read_cnt;

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int file_fd;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    fp = fopen("file_server.c", "rb");
    if (fp == NULL)
        error_handling("fopen() error!");

    serv_sd = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sd == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sd, (const struct sockaddr *) &serv_adr, sizeof(serv_adr)))
        error_handling("bind() error!");

    if (-1 == listen(serv_sd, 5))
        error_handling("listen() error");

    clnt_sd = accept(serv_sd, (struct sockaddr *) &clnt_adr, &clnt_adr_sz);
    if (clnt_sd == -1)
        error_handling("accept() error!");


    while (1) {
        read_cnt = fread((void *) buf, 1, BUF_SIZE, fp);
        if (read_cnt < BUF_SIZE) {
            write(clnt_sd, buf, read_cnt);
            break;
        }
        write(clnt_sd, buf, BUF_SIZE);
    }
    shutdown(clnt_sd, SHUT_WR);
    read(clnt_sd, buf, BUF_SIZE);
    printf("Message from client: %s\n", buf);

    fclose(fp);
    close(clnt_sd);
    close(serv_sd);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
