#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	FILE *fp;

	//ssu_test.txt 파일을 읽기모드로 연다
	if((fp = fopen(fname, "r"))==NULL){
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else{
		printf("Success!\n");
		printf("Opening \"%s\" in \"r\" mode!\n", fname);
	}

	fclose(fp);
	exit(0);
}
