#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(void)
{
    pid_t child1, child2;
    int pid,status;

    //자식 프로세스 child1을 실행한다.
    if((child1 = fork())==0)
        //date명령을 실행시킨다.
        execlp("date","date",(char*)0);

    //자식 프로세스 child2를 실행한다.
    if((child2=fork())==0)
        //who명령을 실행시킨다
        execlp("who","who",(char*)0);

    printf("parent: waiting for children\n");

    //자식프로세스가 종료되기까지 기다린다.
    while((pid=wait(&status))!=-1){
        //자식프로세스 child1이 종료되었을때
        if(child1==pid)
            printf("parent: first child: %d\n",(status >> 8));
        //자식 프로세스 child2가 종료되었을때
        else if(child2==pid)
            printf("parent: second child: %d\n", (status >> 8));
    }

    //자식 프로세스 모두가 종료되었을때 출력
    printf("parent: all children terminated\n");
    exit(0);
}