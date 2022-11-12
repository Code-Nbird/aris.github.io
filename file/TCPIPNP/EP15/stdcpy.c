//
// Created by Administrator on 2022/11/10.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BUF_SIZE 3 // 用最短数组长度构成

int main(int argc, char* argv[]) {
    FILE * fp1;
    FILE * fp2;
    char buf[BUF_SIZE];

    fp1= fopen("news.txt", "r");
    fp2= fopen("cpy.txt", "w");
    if (fp1 == NULL){
        printf("fopen() error!");
        exit(1);
    }
    if (fp2 == NULL){
        printf("fopen() error!");
        exit(1);
    }

    clock_t start = clock();
    while(fgets(buf,BUF_SIZE,fp1)!=NULL)
        fputs(buf,fp2);
    clock_t end = clock();

    printf("time : %lf s \n", (double)(end - start)/CLOCKS_PER_SEC);

    fclose(fp1);
    fclose(fp2);
    return 0;
}
