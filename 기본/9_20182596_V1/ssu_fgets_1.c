#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];

	//표준 입력으로부터 입력 받고 입력 받은 문자를 비교
	while(fgets(buf, BUFFER_SIZE, stdin) != NULL)
		if(fputs(buf, stdout) == EOF){
			fprintf(stderr, "standard output error\n");
			exit(1);
		}

	if(ferror(stdin)) {
		fprintf(stderr, "standard input error\n");
		exit(1);
	}

	exit(0);
}
