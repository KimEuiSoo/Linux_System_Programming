#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void ssu_alarm(int signo);
int main(void) {
    printf("Alarm Setting\n");
    //SIGALR실행시 ssu_alarm 함수실행
    signal(SIGALRM, ssu_alarm);
    //2초후 SIGALRM 실행시킨다
    alarm(2);
    while (1) {
        printf("done\n");
        //시그널을 받을때까지 멈춘다.
        pause();
        alarm(2);
    }
    exit(0);
}

void ssu_alarm(int signo) {
    printf("alarm..!!!\n");
}