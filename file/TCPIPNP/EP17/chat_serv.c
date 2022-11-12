//
// Created by Administrator on 2022/11/10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#define EPOLL_SIZE 10
#define BUF_SIZE 120
#define MAX_CLNT 256

void error_handling(char *msg);

void set_nonblock_mode(int fd);

void send_msg(int sock, char *buf, int len);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];

int main(int argc, char *argv[]) {
    int clnt_sock, serv_sock, sock;
    int epfd, ep_cnt;
    char msg[BUF_SIZE];
    char buf[BUF_SIZE];
    struct sockaddr_in clnt_adr, serv_adr;
    int str_len;
    socklen_t adr_sz;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");
    set_nonblock_mode(serv_sock);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)))
        error_handling("bind() error");

    if (-1 == listen(serv_sock, 5))
        error_handling("listen() error");

    epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
    struct epoll_event ep_events[EPOLL_SIZE];
    while (1) {
        adr_sz = sizeof(clnt_adr);
        ep_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (ep_cnt == -1) {
            printf("epoll error");
            break;
        }
        for (int i = 0; i < ep_cnt; i++) {
            if (ep_events[i].data.fd == serv_sock) {
                /* 接收到客户端连接请求 */
                clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &adr_sz);
                event.data.fd = clnt_sock;
                event.events = EPOLLIN | EPOLLET;
                set_nonblock_mode(clnt_sock);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                clnt_socks[clnt_cnt++] = clnt_sock;
                printf("Client %d connecting ...\n", clnt_sock);
            } else {
                sock = ep_events[i].data.fd;
                while (1) {
                    str_len = read(sock, buf, BUF_SIZE);
                    if (str_len == 0) {
                        /* 接收到 EOF ，客户端断开连接 */
                        close(sock);
                        printf("Client %d exit...\n", sock);

                        for (int i = 0; i < clnt_cnt; i++) {
                            if (sock == clnt_socks[i]) {
                                while (i++ < clnt_cnt - 1)
                                    clnt_socks[i] = clnt_socks[i + 1];
                                break;
                            }
                        }
                        clnt_cnt --;
                        break;
                    } else if (str_len < 0) {
                        if (errno == EAGAIN)
                            break;
                    } else {
                        /* 接收到单个客户端的消息 */
                        send_msg(sock, buf, str_len);
                    }
                }

            }
        }
    }

    return 0;
}

void send_msg(int sock, char *buf, int len) {
    for (int i = 0; i < clnt_cnt; i++) {
        if (sock != clnt_socks[i])
            write(clnt_socks[i], buf, len);
    }
}


void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void set_nonblock_mode(int fd) {
    int flag = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}