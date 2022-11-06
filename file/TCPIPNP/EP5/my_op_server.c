#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SZ 1024

void error_handling(const char *string);

int handler(int opn, char pkg[BUF_SZ]);


int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    char msg[BUF_SZ];
    int str_len, recv_cnt, recv_len;
    int opn, res;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len;

    if (argc != 2) {
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error!");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if (-1 == bind(serv_sock, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)))
        error_handling("bind() error!");

    if (-1 == listen(serv_sock, 5))
        error_handling("listen() error!");

    while (1) {
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
            error_handling("accept() error!");
        read(clnt_sock, &opn, sizeof(opn));
        recv_len = 0;
        while ((opn * sizeof(int) + sizeof(int)) > recv_len) {
            recv_cnt = read(clnt_sock, msg, BUF_SZ);
            recv_len += recv_cnt;
        }
        res = handler(opn, msg);
        write(clnt_sock, &res, sizeof(res));
        close(clnt_sock);
    }
    return 0;
}

int handler(int opn, char pkg[BUF_SZ]) {
    char op = *(char *) (pkg);
    int res = *(int *) (pkg + 4);
    for (int i = 2; i <= opn; i++) {
        int opa = *(int *) (pkg + 4 * i);
        switch (op) {
            case '+' :
                res += opa;
                break;
            case '-' :
                res -= opa;
                break;
            case '*':
                res *= opa;
                break;
            default:
                res *= 1;
                break;
        }
    }
    return res;
}

void error_handling(const char *string) {
    fputs(string, stderr);
    fputc('\n', stderr);
    exit(1);
}
