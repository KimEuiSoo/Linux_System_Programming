#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>

int main()
{
    int flag;

    //인자 부족으로 에러가 발생한다.
    if((flag=fcntl(STDOUT_FILENO,F_DUPFD))==-1){
        fprintf(stderr,"Error : Checking CLOSEE_ON_EXEC\n");
        exit(1);
    }

    printf("CLOSE ON EXEC flag is = %d\n",flag);
    exit(0);
}