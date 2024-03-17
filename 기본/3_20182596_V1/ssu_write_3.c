#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

int main(int argc, char *argv[]){
	//employee 구조체 record를 선언
	struct ssu_employee record;
	int fd;

	if(argc < 2){
		fprintf(stderr, "usage : %s file\n", argv[0]);
		exit(1);
	}
	
	//argv[1]두번쨰 인자를 연다 / 없으면 만든다 권한은 rw_r_____만든다
	if((fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0640))<0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	//반복하여 record에 이름과 봉급을 데이터를 받는다고서 ssu_employeefile에 write한다. 이름에 .을 입력하면 반복문을 탈출
	while(1){
		printf("Enter employee name <SPACE> salary: ");
		scanf("%s", record.name);

		if(record.name[0] == '.')
			break;

		scanf("%d", &record.salary);
		record.pid = getpid();
		write(fd, (char *)&record, sizeof(record));
	}
	//파일을 닫는다
	close(fd);
	exit(0);
}
