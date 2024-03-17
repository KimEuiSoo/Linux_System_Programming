#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
int main(int argc, char *argv[])
{
    //인자가 2개 인지 확인
    if (argc != 2) {
        fprintf(stderr, "usage: %s [Process ID]\n", argv[0]);
        exit(1);
    }
    else
    	//인자로 들어온 첫번째 프로그램에 SIGKILL을 보냄
        kill(atoi(argv[1]), SIGKILL);
    exit(0);
}
