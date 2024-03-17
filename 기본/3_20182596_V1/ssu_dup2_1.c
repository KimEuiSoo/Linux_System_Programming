#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname = "ssu_test.txt";
	int fd;

	//ssu_test.txt파일을 만든다 권한 rw_rw_rw_
	if((fd = creat(fname, 0666)) < 0){
		printf("creat error for %s\n", fname);
		exit(1);
	}
	
	//맨처음 출력될 데이터
	printf("First printf is on the screen.\n");
	//ssu_test.txt파일에 출력 텍스트를 담는다.
	dup2(fd, 1);
	//cat <filename>으로 호출하여 텍스트를 출력
	printf("Second printf is in this file.\n");
	exit(0);
}
