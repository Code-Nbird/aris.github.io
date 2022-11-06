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
    int str_len, i;
    socklen_t adr_sz;
    struct sockaddr_in my_adr, your_adr;

    if (argc != 2) {
        fprintf(stdout, "Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handling("socket() error!");
    }

    memset(&my_adr, 0, sizeof(my_adr));
    my_adr.sin_family = AF_INET;
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(sock, (struct sockaddr *) &my_adr, sizeof(my_adr)))
        error_handling("bind error!");

    for (i = 0; i < 3; i++) {
        sleep(5);
        adr_sz = sizeof(your_adr);
        recvfrom(sock, message, BUF_SIZE, 0,
                 (struct sockaddr *) &your_adr, &adr_sz);
        printf("Message %d: %s \n", i+1, message);
    }
    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}