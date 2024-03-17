#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define LINE_MAX 2048
static void ssu_alarm(int signo);
int main(void)
{
    char buf[LINE_MAX];
    int n;
    //SIGALRM실행시 ssu_alarm 실행하고 SIG_ERR면 오류를 출력
    if (signal(SIGALRM, ssu_alarm) == SIG_ERR) {
        fprintf(stderr, "SIGALRM error\n");
        exit(1);
    }
    //10초후 SIGALRM 실행시킨다
    alarm(10);
    //buf에 LINE_MAX만큼 읽어오고 읽어오는게 없으면 에러
    if ((n = read(STDIN_FILENO, buf, LINE_MAX)) < 0) {
        fprintf(stderr, "read() error\n");
        exit(1);
    }
    //즉시 SIGALRM 실행시킨다.
    alarm(0);
    write(STDOUT_FILENO, buf, n);
    exit(0);
}

static void ssu_alarm(int signo) {
    printf("ssu_alarm() called!\n");
}