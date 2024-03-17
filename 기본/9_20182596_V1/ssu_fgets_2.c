#include <stdio.h>
#include <stdlib.h>

#define BUFFER_MAX 256

int main(void)
{
	char command[BUFFER_MAX];
	char *prompt = "Prompt>>";

	while(1){
		//변수prompt값을 출력
		fputs(prompt, stdout);

		//입력받을 값을 받는다
		if(fgets(command, sizeof(command), stdin) == NULL)
			break;
		
		//시스템에서 command에 입력받은값을 출력
		system(command);
	}

	fprintf(stdout, "Good bye...\n");
	fflush(stdout);
	exit(0);
}
