#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char glob_str[] = "write to standard output\n";
int glob_val = 10;

int main(void){
	pid_t pid;
	int loc_val;
	loc_val = 100;
	//write는 버퍼링 되지 않고 한번에 출력
	if(write(STDOUT_FILENO,glob_str, sizeof(glob_str)-1) != sizeof(glob_str)-1){
		fprintf(stderr, "write error\n");
		exit(1);
			}

	printf("before fork\n");

	//프로세스 생성
	if((pid = fork()) < 0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식프로세스
	else if(pid == 0){
		glob_val++;
		loc_val++;
	}
	//부모프로세스가 3초동안 기다린다.
	else
		sleep(3);

	printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);
	exit(0);

}
