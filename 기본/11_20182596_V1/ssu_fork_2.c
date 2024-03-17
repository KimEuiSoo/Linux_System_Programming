#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	pid_t pid;
	char character, first, last;
	long i;
	//프로세스 확인
	if((pid = fork()) > 0){
		first = 'A';
		last = 'Z';
	}
	//자식프로세스 시작
	else if(pid == 0){
		first = 'a';
		last = 'z';
	}
	else{
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	//문자를 a를 z까지 write
	for(character = first; character <= last; character++){
		for(i=0; i<=100000; i++);
		write(1, &character, 1);
	}

	printf("\n");
	exit(0);

}
