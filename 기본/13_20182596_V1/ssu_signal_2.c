#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
static void ssu_signal_handler(int signo);
int main(void) {
    //SIGINT가 들어올시 ssu_signal_handler을 실행한다
    if (signal(SIGINT, ssu_signal_handler) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGINT\n");
        exit(EXIT_FAILURE);
    }
    //SIGTERM가 들어올시 ssu_signal_handler을 실행한다.
    if (signal(SIGTERM, ssu_signal_handler) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGTERM\n");
        exit(EXIT_FAILURE);
    }
    //SIGPROF가 들어올시 기본 행동을 실행한다다
    if (signal(SIGPROF, SIG_DFL) == SIG_ERR) {
        fprintf(stderr, "cannot reset SIGPROF\n");
        exit(EXIT_FAILURE);
    }
    //SIGHUP가 들어올시 시그널 무시한다.
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "cannot ignore SIGHUP\n");
        exit(EXIT_FAILURE);
    }
    while (1)
        pause();
    exit(0);
}
static void ssu_signal_handler(int signo) {
    //sigint가 들어올시 내용출력한다
    if (signo == SIGINT)
        printf("caught SIGINT\n");
    //sigterm 발생시 내용 출력한다.
    else if (signo == SIGTERM)
        printf("caught SIGTERM\n");
    else {
        //이상한 시그널이 들어와 실패하여 종료됨을 알림
        fprintf(stderr, "unexpected signal\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
