#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void ssu_charatatime(char *str);

int main(void){
	pid_t pid;
	//프로세스 확인
	if((pid = fork()) < 0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식프로세스 실행
	else if(pid==0)
		ssu_charatatime("output from child\n");
	//부모프로세스
	else
		ssu_charatatime("output from parent\n");

	exit(0);
}

static void ssu_charatatime(char *str){
	char *ptr;
	int print_char;
	
	setbuf(stdout, NULL);

	for(ptr=str; (print_char = *ptr++) != 0;){
		putc(print_char, stdout);
		usleep(10);
	}
}
