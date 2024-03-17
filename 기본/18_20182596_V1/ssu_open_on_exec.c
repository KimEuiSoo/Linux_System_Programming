#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>

int main()
{
    int fd;
    int val;
    //exec_copy.txt 파일을 생성한다.
    fd = open("exec_copy.txt",O_CREAT);
    //loop를 실행
    execl("/home/kimeuisoo/Linux/project18/loop","./loop",NULL);
    exit(0);
}
