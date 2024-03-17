#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void){
	//현재 디렉토리에 있는 텍스트 파일
	char *fname = "ssu_test.txt";
	int fd;

	//현재 디렉토리에 있는 텍스트 파일 읽기전용으로 오픈
	//Error일 경우에 -1로 걸러진다.
	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n",fname);
		exit(1);
	}
	else
		printf("Success!\nFilename : %s\nDescriptor : %d\n", fname, fd);

	exit(0);
}
