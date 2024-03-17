#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void ssu_echo_exit(int status);

int main(void)
{
    pid_t pid;
    int status;

    //자식프로세스 생성
    if((pid=fork())<0){
        fprintf(stderr,"fork error\n");
        exit(1);
    }
    //자식을 7을 반환하고 종료시킴
    else if(pid==0)
        exit(7);
    //자식 프로세스가 종료되기를 기다림
    if(wait(&status)!=pid){
        fprintf(stderr,"wait error\n");
        exit(1);
    }

    ssu_echo_exit(status);
    //자식프로세스 생성
    if((pid=fork())<0){
        fprintf(stderr,"fork error\n");
        exit(1);
    }
    //자식은 abort함수를 실행
    else if(pid==0)//
        abort();
    //자식 프로세스가 종료되길 기다림
    if(wait(&status) != pid){
        fprintf(stderr,"wait error\n");
        exit(1);
    }

    //status에 대한 정보를 보여줌
    ssu_echo_exit(status);

    //자식 프로세스 생성
    if((pid=fork())<0){
        fprintf(stderr,"fork error\n");
        exit(1);
    }
    else if(pid==0)
        status /= 0;
    //자식 프로세스가 종료되길 기다림
    if(wait(&status)!=pid){
        fprintf(stderr,"wait error\n");
        exit(1);
    }

    ssu_echo_exit(status);
    exit(0);
}

void ssu_echo_exit(int status){
    //자식 프로세스가 정상 종료되었을 떄
    if(WIFEXITED(status))
        printf("normal termination, exit status = %d\n",WEXITSTATUS(status));
        //자식 프로세스가 비정상 종료되었을 떄
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n",WTERMSIG(status),
#ifdef WCOREDUMP
            WCOREDUMP(status) ? " (core file generated)" : "");
#else
            "");
#endif
    else if(WIFSTOPPED(status))
        ////자식프로세스가 현재 종료되었다는것을 알려준다.
        printf("child stopped, signal number = %d\n",WSTOPSIG(status));
}
