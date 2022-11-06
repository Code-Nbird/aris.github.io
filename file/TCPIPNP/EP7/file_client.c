//
// Created by Administrator on 2022/11/6.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SZ 30

void error_handling(char *msg);

int main(int argc, char *argv[]) {
    int sd;
    char buf[BUF_SZ];
    int read_cnt;
    FILE *fp;

    struct sockaddr_in serv_adr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    fp = fopen("recv_file.dat", "wb");
    if (fp == NULL)
        error_handling("fopen() error!");

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));
    if (-1 == connect(sd, (const struct sockaddr *) &serv_adr, sizeof(serv_adr)))
        error_handling("connect() error!");


    while ((read_cnt = read(sd, buf, BUF_SZ)) != 0) {
        fwrite((void *) buf, 1, read_cnt, fp);
    }

    puts("Received file data");
    write(sd, "Thank you", 10);
    fclose(fp);
    close(sd);
    return 0;
}


void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
