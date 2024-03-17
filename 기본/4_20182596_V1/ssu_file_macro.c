#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	struct stat file_info;
	char *str;
	int i;
	for(int i=1; i<argc; i++){
		printf("name = %s, type = ", argv[i]);

		if(lstat(argv[i], &file_info)<0){
			fprintf(stderr, "lstat error\n");
			continue;
		}
		//일반 파일일때
		if(S_ISREG(file_info.st_mode))
			str = "regular";
		//디렉토리일때
		else if(S_ISDIR(file_info.st_mode))
			str = "directory";
		//블록특수파일일때
		else if(S_ISCHR(file_info.st_mode))
			str = "character special";
		//문자파일일때
		else if(S_ISBLK(file_info.st_mode))
			str = "Black special";
		//FIFO일때
		else if(S_ISFIFO(file_info.st_mode))
			str = "FIFO";
		//심볼릭링크일때
		else if(S_ISLNK(file_info.st_mode))
			str = "symbolic link";
		//소켓일때
		else if(S_ISSOCK(file_info.st_mode))
			str = "SOCK";
		else
			str = "unknown mode";

		printf("%s\n", str);
	}
	exit(0);
}
