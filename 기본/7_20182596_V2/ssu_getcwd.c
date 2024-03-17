#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

int main(void){
	char *pathname;

	//현재작업 디렉토리를 /home/kimeuisoo로 변경
	if(chdir("/home/kimeuisoo") < 0){
		fprintf(stderr, "chdir error\n");
		exit(1);
	}

	pathname = malloc(PATH_MAX);

	//작업디렉토리를 얻고 작업 디렉토리 변경 확인
	if(getcwd(pathname, PATH_MAX) == NULL){
		fprintf(stderr, "getcwd error\n");
		exit(1);
	}

	printf("current directory = %s\n", pathname);
	exit(0);
}
