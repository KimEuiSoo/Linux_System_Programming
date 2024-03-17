#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(void)
{
    //자식 프로세스일 경우 실행한다.
    if (fork() == 0)
            execl("/bin/echo", "echo", "this is", "message one", (char *)0);//위의 명령어를 실행

    //자식 프로세스일 경우 실행한다.
    if (fork() == 0)
        execl("/bin/echo", "echo", "this is", "message Two", (char *)0);//위의 명령어를 실행

    printf("parent: waiting for children\n");

    //자식 프로세스가 끝나고 기다린다.
    while (wait((int *)0) != -1);
    //자식 프로세스가 끝남
    printf("parent: all children terminated\n");
    exit(0);
}