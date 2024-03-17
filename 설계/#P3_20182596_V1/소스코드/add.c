#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int tOption = false;

int main(int argc, char *argv[]){
	int tmp=0;
	while((tmp = getopt(argc, argv, "t")) != -1){
		switch(tmp){
			case 't':
				tOption = true;
				printf("t\n");
				break;
			default:
				printf("Usage:\n");
				printf("  > add <FILENAME> [OPTION]\n");
				printf("\t-t : Restart monitoring at <TIME> intervals\n");
				exit(1);
		}
	}
}
