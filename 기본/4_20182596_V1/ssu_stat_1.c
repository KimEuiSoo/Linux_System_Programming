#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
	struct stat statbuf;

	//인자의 갯수가 2개가 아니면 오류
	if(argc != 2){
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	//호출된 두번째 인자의 버퍼의 크기가 0보다 작으면 오류
	if((stat(argv[1], &statbuf)) < 0){
		fprintf(stderr, "Stat error\n");
		exit(1);
	}

	printf("%s is %ld bytes\n", argv[1], statbuf.st_size);
	exit(0);
}
