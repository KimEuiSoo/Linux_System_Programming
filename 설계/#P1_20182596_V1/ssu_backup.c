#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <limits.h>
#include <time.h>

#define Temp 1000
#define ADD 1
#define REMOVE 2
#define RECOVER 3
#define HELP 4
#define EXIT 5
#define LS 6
#define VI 7
#define BUFSIZE 1024*16

//input 데이터를 받는함수
void inputData(void);
//Add실행시키는 함수
void add_command(char* input);
//Remode실행시키는 함수
void remove_command(char* input);
//Recover실행시키는 함수
void recover_command(char* input);
//Exit실행시키는 함수
void exit_command(void);
//Help실행시키는 함수
void help_command(void);
//ls실행시키는 함수
void ls_command(void);
//vi실행시키는 함수
void vi_command(void);
//Input 데이터를 비교하는 함수
int input_command(char* input);

//현재위치한 디렉토리 주소
static char* current_path;
//md5를 사용할지 sha1을 사용할지 저장해둔 값
static char* hash_data;

int main(int argc, char** argv){
	char md5_hash[33];

	current_path = getcwd(NULL, BUFSIZ);

	if(argc==1){
		printf("Usage: ssu_backup <md5 | sha1>\n");
		exit(1);
	}
	else if(!strcmp(argv[1], "md5")){
		hash_data = "md5";
		inputData();
	}
	else if(!strcmp(argv[1], "sha1")){
		hash_data = "sha1";
		inputData();
	}
}

void add_command(char* input){
	char* data[8];
	char* token = strtok(input, " ");
	char* hash = hash_data;
	int i=0;

	while(token != NULL){
		data[i++] = token;
		token = strtok(NULL, " ");
	}

	pid_t pid;
	if((pid = fork()) < 0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		if(i==1)
			execl("./add", "./add", data[0], hash, NULL);
		else if(i==2)
			execl("./add", "./add", data[0], data[1], hash, NULL);
		else if(i==3)
			execl("./add", "./add", data[0], data[1], data[2], hash, NULL);
	}
	else wait(NULL);


	return;
}

void remove_command(char* input){
	printf("remove\n");
	exit(1);
}

void recover_command(char* input){
	char* data[8];
	char* token = strtok(input, " ");
	int i=0;

	while(token != NULL){
		data[i++] = token;
		token = strtok(NULL, " ");
	}

	pid_t pid;
	if((pid = fork()) < 0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		if(i==1)
			execl("./recover", "./recover", data[0], NULL);
		else if(i==2)
			execl("./recover", "./recover", data[0], data[1], NULL);
		else if(i==3)
			execl("./recover", "./recover", data[0], data[1], data[2], NULL);
		else if(i==4)
			execl("./recover", "./recover", data[0], data[1], data[2], data[3], NULL);
		else if(i==5)
			execl("./recover", "./recover", data[0], data[1], data[2], data[3], data[4],NULL);
	}
	else wait(NULL);
}

void ls_command(){
	pid_t pid;
	if((pid = fork()) < 0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		execl("/bin/ls", "ls", NULL);
		exit(0);
	}
	else wait(NULL);
}

void vi_command(){
	pid_t pid;

	if((pid = fork())<0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		execl("/bin/vi", "vi","ssu_backup.c", NULL);
	}
	else wait(NULL);
}

void help_command(){
	pid_t pid;
	if((pid = fork())<0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		execl("./help","./help",NULL);
		exit(0);
	}
	else wait(NULL);
}

void exit_command(){
	exit(1);
}

int input_command(char *input){
	int char_d;
	int i=0;
	while((char_d = getc(stdin))!='\n'){
		input[i] = char_d;
		i++;
	}
	input[i]='\0';
	if(input[0]=='h' && input[1]=='e' && input[2]=='l' && input[3]=='p')
		return HELP;
	else if(input[0]=='e' && input[1]=='x' && input[2]=='i' && input[3]=='t'){
		return EXIT;
	}
	else if(input[0]=='a' && input[1]=='d' && input[2]=='d'){
		return ADD;
	}
	else if(input[0]=='r' && input[1]=='e' && input[2]=='m' && 
			input[3]=='o' && input[4]=='v' && input[5]=='e'){
		return REMOVE;
	}
	else if(input[0]=='r' && input[1]=='e' && input[2]=='c' && 
			input[3]=='o' && input[4]=='v' && input[5]=='e' &&
			input[6]=='r'){
		return RECOVER;
	}
	else if(input[0]=='l' && input[1]=='s'){
		return LS;
	}
	else if((input[0]=='v' && input[1]=='i')||(input[0]=='v' &&
				input[1]=='i' && input[2]=='m')){
		return VI;
	}
}


void inputData(){
	char input[Temp];
	int command;

	char *buf;
	char path[256] = "/home";
	buf = getlogin();
	strcat(path, "/");
	strcat(path, buf);
	strcat(path, "/");
	strcat(path, "backup");

	struct stat st = {0};

	if(stat(path, &st) == -1){
		if(mkdir(path,0775) == -1){
		}
	}

	while(1){
		printf("20182596>");
		command = input_command(input);

		if(command == HELP)	help_command();
		else if(command == EXIT) exit_command();
		else if(command == ADD)	add_command(input);
		else if(command == REMOVE)	remove_command(input);
		else if(command == RECOVER)	recover_command(input);
		else if(command == LS) ls_command();
		else if(command == VI) vi_command();
		else help_command();
	}
}
