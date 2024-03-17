#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	//인자의 갯수가 3개미만이면 오류
	if(argc < 3){
		fprintf(stderr, "usage : %s <file1> <file2>\n", argv[0]);
		exit(1);
	}

	//첫번째 인자와 두번째 인자를 연결)
	if(link(argv[1], argv[2]) == -1){
		fprintf(stderr, "link error for %s\n", argv[1]);
		exit(1);
	}

	exit(0);
}
