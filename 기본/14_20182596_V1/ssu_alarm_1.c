#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void ssu_signal_handler(int signo);
int count = 0;
int main(void)
{
    //SIGALRM으로 ssu_signal_handler을 실행
    signal(SIGALRM, ssu_signal_handler);
    //1초후 SIGALRM 실행
    alarm(1);
    while(1);
    exit(0);
}

void ssu_signal_handler(int signo) {
    //알람을 증가시킨다.
    printf("alarm %d\n", count++);
    //1초후 SIGALRM 실행시킨다
    alarm(1);
}