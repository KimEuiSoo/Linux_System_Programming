#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(void)
{
    sigset_t sig_set;
    int count;
    //sig_set을 초기화 한다.
    sigemptyset(&sig_set);
    //sig_set에 SIGINT를 추가한다.
    sigaddset(&sig_set, SIGINT);
    //sig_set에 마스크를 SIG_BLOCK를 설정해준다.
    sigprocmask(SIG_BLOCK, &sig_set, NULL);
    for (count = 3 ; 0 < count ; count--) {
        printf("count %d\n", count);
        sleep(1);
    }
    printf("Ctrl-C에 대한 블록을 해제\n");
    //sig_set에 마스크을 SIG_BLOCK을 해제한다.
    sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
    printf("count중 Ctrl-C입력하면 이 문장은 출력 되지 않음.\n");
    while (1);
    exit(0);
}