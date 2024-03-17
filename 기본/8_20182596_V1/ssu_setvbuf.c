#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "/dev/pts/19";
	FILE *fp;

	//fname파일을 쓰기로 연다.
	if((fp = fopen(fname, "w")) == NULL){
		fprintf(stderr, "fopen error for %s", fname);
		exit(1);
	}

	ssu_setbuf(fp, buf);
	fprintf(fp, "Hello, ");
	sleep(1);
	fprintf(fp, "UNIX!!");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	//NULL을 넣었기 때문에 버퍼에 넣지 않고 호출
	ssu_setbuf(fp, NULL);
	fprintf(fp, "How");
	sleep(1);
	fprintf(fp, " ARE");
	sleep(1);
	fprintf(fp, " YOU?");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	exit(0);
}

void ssu_setbuf(FILE *fp, char *buf){
	size_t size;
	int fd;
	int mode;

	fd = fileno(fp);

	//현재 표준 입력에 접속된 터미널 장치 파일 이름을 받는다.
	if(isatty(fd))
		mode = _IOLBF;
	else
		mode = _IOFBF;

	if(buf == NULL){
		mode = _IONBF;
		size = 0;
	}
	else
		size = BUFFER_SIZE;

	//버퍼를 설정 해준다.
	setvbuf(fp, buf, mode, size);
}
