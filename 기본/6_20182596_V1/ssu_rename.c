#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	
	//인자가 세개가 아니면 오류
	if(argc != 3){
		fprintf(stderr, "usage : %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	//두번째 인자를 읽기전용으로 오픈한다
	if((fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else
		close(fd);

	if(rename(argv[1], argv[2]) < 0){
		fprintf(stderr, "rename error\n");
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0)
		printf("second open error for %s\n", argv[1]);
	else{
		fprintf(stderr, "it's very strange!\n");
		exit(1);
	}

	if((fd = open(argv[2], O_RDONLY)) < 0){
		fprintf(stderr, "thrid open error for %s\n", argv[2]);
		exit(1);
	}

	printf("Everything is good!\n");
	exit(0);
}
