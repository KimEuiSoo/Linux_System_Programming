#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char *fname = "ssu_dump.txt";

	//해당 파일을 연다.
	if(open(fname, O_RDWR) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	
	//파일의 링크를 해제한다.
	if(unlink(fname) < 0){
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	printf("File unlinked\n");
	//0.02초 딜레이
	sleep(20);
	printf("Done\n");
	exit(0);
}
