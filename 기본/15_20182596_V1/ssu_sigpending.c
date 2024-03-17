#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(void)
{
    sigset_t pendingset;
    sigset_t sig_set;
    int count = 0;
    //sig_set에 모든 시그널을 포함시킨다.
    sigfillset(&sig_set);
    sigprocmask(SIG_SETMASK, &sig_set, NULL);
    while (1) {
        printf("count: %d\n", count++);
        sleep(1);
        //펜딩 중이거나 블록되었을때 현재 팬딩중인 시그널들의 집합을 pendingset에 저장한다.
        if (sigpending(&pendingset) == 0) {
            //pendingset에 SIGINT가 있는지 체크한다.
            if (sigismember(&pendingset, SIGINT)) {
                printf("SIGINT가 블록되어 대기 중. 무한 루프를 종료.\n");
                break;
            }
        }
    }
    exit(0);
}