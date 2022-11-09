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

void read_childproc(int sig);

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in clnt_adr, serv_adr;
    int fds[2];

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len,state;
    char buf[BUF_SZ];
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

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

    if (-1 == listen(serv_sock, 5))
        error_handling("listen() error!");

    pipe(fds);
    pid = fork();
    if (pid == 0)
    {
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SZ];
        int i,len;
        for (i = 0; i < 10; i ++)
        {
            len = read(fds[0], msgbuf, BUF_SZ);
            fwrite((void*)msgbuf,1,len,fp);
        }
        fclose(fp);
        return 0;
    }
    while (1) {
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            continue;
        else
            puts("new client connected...");
        pid = fork();
        switch (pid) {
            case -1:
                close(clnt_sock);
                continue;
            case 0:
                close(serv_sock);
                while (str_len = read(clnt_sock, buf, BUF_SZ))
                {
                    write(clnt_sock, buf, str_len);
                    write(fds[1], buf, str_len);
                }
                close(clnt_sock);
                puts("client disconnected...");
                return 0;
            default:
                close(clnt_sock);
                break;
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig) {
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}