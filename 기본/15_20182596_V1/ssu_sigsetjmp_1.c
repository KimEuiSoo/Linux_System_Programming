#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
void ssu_signal_handler(int signo);
jmp_buf jump_buffer;

int main(void) {
    //SIGINT실행시 ssu_signal_handler를 실행한다.
    signal(SIGINT, ssu_signal_handler);
    while (1) {
        //jump_buffer에 스택값들을 env에 저장, longjmp할 곳을 지정
        if (setjmp(jump_buffer) == 0) {
            printf("Hit Ctrl-c at anytime ...\n");
            pause();
        }
    }
    exit(0);
}
void ssu_signal_handler(int signo) {
    char character;
    //signo발생시 무시한다.
    signal(signo, SIG_IGN);
    printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
    character = getchar();
    if (character == 'y' || character == 'Y')
        exit(0);
    else {
        //SIGINT발생시 ssu_signal_handler을 실행한다.
        signal(SIGINT, ssu_signal_handler);
        //jump_buffer로 이동한다.
        longjmp(jump_buffer, 1);
    }
}