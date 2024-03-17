#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	int testfd;
	int fd;

	fd = open("test.txt", O_CREAT);

	testfd = fcntl(fd, F_DUPFD, 5);
	printf("testfd : %d\n", testfd);
	testfd = fcntl(fd, F_DUPFD, 5);
	printf("testfd : %d\n", testfd);
	
	getchar();
}
