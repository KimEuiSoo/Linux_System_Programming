#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	//인자의 갯수가 3개가 아니면 오류
	if(argc != 3){
		fprintf(stderr, "usage: %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	//두번쨰 인자를 세번째 인자와 링크 연결
	if(link(argv[1], argv[2]) < 0){
		fprintf(stderr, "link error\n");
		exit(1);
	}
	else
		printf("step1 passed.\n");

	//두번째 인자 삭제
	if(remove(argv[1]) < 0){
		fprintf(stderr, "remove error\n");
		//두번째 인자를 삭제 못했으면 세번째 인자 삭제
		remove(argv[2]);
		exit(1);
	}
	else
		printf("step2 passed.\n");

	printf("Success!\n");
	exit(0);
}
