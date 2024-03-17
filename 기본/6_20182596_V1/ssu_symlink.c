#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	//인자가 3개가 아니면 오류
	if(argc != 3){
		fprintf(stderr, "usage: %s <actualname> <symname>\n", argv[0]);
		exit(1);
	}

	//심벌릭링크 파일을 생성한다.
	if(symlink(argv[1], argv[2]) < 0){
		fprintf(stderr, "symlink error\n");
		exit(1);
	}
	else
		printf("symlink: %s -> %s\n", argv[2], argv[1]);

	exit(0);
}
