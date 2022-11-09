//
// Created by Administrator on 2022/11/8.
//

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <wait.h>

#define BUF_SZ 30


void error_handling(char *msg);

void read_childproc(int sig) {
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}


int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in clnt_adr, serv_adr;
    socklen_t clnt_adr_sz;
    int str_len;
    char buf[BUF_SZ];

    pid_t pid;
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sock, (const struct sockaddr *) &serv_adr, sizeof(serv_adr)))
        error_handling("bind() error!");

    if (-1 == listen(serv_sock,5))
        error_handling("listen() error!");

    while(1) {
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1)
            error_handling("accept() error!");
        pid = fork();
        switch (pid) {
            case -1:
                error_handling("fork() error!");
            case 0:
                while(str_len = read(clnt_sock,buf,BUF_SZ))
                    write(clnt_sock,buf,str_len);
                close(clnt_sock);
                close(serv_sock);
                exit(0);
            default:
                break;
        }
    }

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
