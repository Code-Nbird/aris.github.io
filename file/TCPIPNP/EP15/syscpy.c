//
// Created by Administrator on 2022/11/10.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 3  // 用最短数组长度构成

int main(int argc, char* argv[]) {
    int fd1, fd2;
    int len;
    char buf[BUF_SIZE];

    fd1 = open("news.txt", O_RDONLY);
    fd2 = open("cpy.txt", O_WRONLY | O_CREAT | O_TRUNC);

    clock_t start = clock();
    while((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);
    clock_t end= clock();
    printf("time : %lf s\n", (double)(end - start)/CLOCKS_PER_SEC);

    close(fd1);
    close(fd2);
    return 0;
}