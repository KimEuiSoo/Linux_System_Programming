#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2;
	
	//ssu_test.txt파일의 권한이 rw_r__r__로 연다
	if((fd1 = open(fname, O_RDONLY, 0644)) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	
	fd2 = dup(fd1);
	//12번째 문자까지 읽는다
	count = read(fd1, buf, 12);
	//버퍼에 담는다
	buf[count] = 0;
	//출력
	printf("fd1's printf : %s\n", buf);
	//1~13번쨰 이후까지 밀어준다
	lseek(fd1, 1, SEEK_CUR);
	//14번째부터 12개의 문자까지 읽어준다
	count = read(fd2, buf, 12);
	//버퍼에 담는다
	buf[count] = 0;
	//출력
	printf("fd2's printf : %s\n",buf);
	exit(0);
}
