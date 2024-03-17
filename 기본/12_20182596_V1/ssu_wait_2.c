#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define EXIT_CODE 1
int main(void)
{
    pid_t pid;
    int ret_val, status;
    //자식 프로세스를 실행
    if ((pid = fork()) == 0) {
        printf("child: pid = %d ppid = %d exit_code = %d\n",
        //자식 프로세스의 pid, ppid를 출력해준다.
        getpid(), getppid(), EXIT_CODE);
        exit(EXIT_CODE);
    }
    printf("parent: waiting for child = %d\n", pid);
    //자식 프로세스가 종료되기를 기다린다
    ret_val = wait(&status);
    //종료 프로세스의 pid값을 가져온다
    printf("parent: return value = %d, ", ret_val);
    printf(" child's status = %x", status);
    printf(" and shifted = %x\n", (status >> 8));
    exit(0);
}