#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {
    printf("ssu_signal_handler control\n");
}

int main(void) {
    struct sigaction sig_act;
    sigset_t sig_set;
    //sig_act.sa_mask를 초기화 시켜준다.
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    //핸들러를 ssu_signal_handler로 설정한다.
    sig_act.sa_handler = ssu_signal_handler;
    //SIGUSR1이 들어왔을때 sig_act을 하도록 실행한다.
    sigaction(SIGUSR1, &sig_act, NULL);
    printf("before first kill()\n");
    kill(getpid(), SIGUSR1);
    //sig_set을 초기화 시켜준다.
    sigemptyset(&sig_set);
    //sig_set에 SIGUSR1을 추가한다.
    sigaddset(&sig_set, SIGUSR1);
    sigprocmask(SIG_SETMASK, &sig_set, NULL);
    printf("before second kill()\n");
    kill(getpid(), SIGUSR1);
    printf("after second kill()\n");
    exit(0);
}