#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include "ssu_monitor.h"

FILE *log_fp;
char *ID = "20182596";
char *monitor_list = "monitor_list.txt";
char *tree_tab = "│   ";
char *tree_mid = "├── ";
char *tree_end = "└── ";
volatile sig_atomic_t signal_received = 0;
int tOption = false;
static int init_dae = 0;

int main(int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(stderr, "usage : %s\n", argv[0]);
		exit(1);
	}
	ssu_prompt();
	return 0;
}

void ssu_prompt(void){
	while (1) {
		char ch;
		char command[BUFLEN][BUFLEN]={};
		char *argv[5]={"\0"};
		int argc=0;
		char *token;
		int num=0;

		printf("%s> ", ID);

		//입력받은 데이터 값입니다.
		while((ch=getchar())!='\n'){
			if(ch==' '){
				command[argc][num]='\0';
				num=0;
				argc++;
				continue;
			}
			//띄워쓰기가 아닌 데이터는 command에 집어넣습니다.
			command[argc][num++]=ch;
		}
		//command로 입력받은 데이터 인자를 argv에 옮깁니다.
		for(int i=0; i<=argc; i++)
			argv[i] = command[i];

		argc++;
		if (execute_command(argc,argv) == 1)
			break;
	}
}
//commad입력받는 함수
int execute_command(int argc, char *argv[]) {
	//add함수
	if (!strcmp(argv[0], "add")) {
		execute_add(argc, argv);
	} else if (!strcmp(argv[0], "delete")) {//delete 호출하는 함수
		execute_delete(argc, argv);	
	} else if (!strcmp(argv[0], "tree")) { //tree 호출하는함수
		execute_tree(argc, argv);
	} else if (!strcmp(argv[0], "help")) { //help 호출하는 함수
		execute_help(argc, argv);	
	} else if (!strcmp(argv[0], "exit")) { //exit 호출하는 함수
		execute_exit(argc, argv);
		return 1;
	} else {
		fprintf(stderr, "wrong command in prompt\n");
		execute_help(argc, argv);
	}
	return 0;
}

//add 실행시키는 함수입니다.
void execute_add(int argc, char *argv[]) {
	char path[BUFLEN];
	char real_path[BUFLEN];
	char line[BUFLEN];
	time_t mn_time;
	struct stat statbuf;
	FILE *fp;
	pid_t pid;

	bzero(line, BUFLEN);
	//인자가 3개거나 4개면 add사용법 호출
	if (argc != 2 && argc != 4) {
		printf("Usage:\n");
		printf("  > add <FILENAME> [OPTION]\n");
		printf("\t-t : Restart monitoring at <TIME> intervals\n");		
		return;
	}


	strcpy(path, argv[1]);

	//파일이 없으면 add사용법 호출
	if (stat(path, &statbuf) < 0) {
		printf("Usage:\n");
		printf("  > add <FILENAME> [OPTION]\n");
		printf("\t-t : Restart monitoring at <TIME> intervals\n");		
		return;
	}
	//해당 파일이 폴더가 아니면 add사용법 호출
	if (!S_ISDIR(statbuf.st_mode)) {
		printf("Usage:\n");
		printf("  > add <FILENAME> [OPTION]\n");
		printf("\t-t : Restart monitoring at <TIME> intervals\n");		
		return;
	}

	// '/'로 시작하지 않으면 절대경로로 만들어준다.
	if (path[0] != '/')
		realpath(path, real_path);
	else	//절대 경로이기 떄문에 real_path에 넣어준다.
		strcpy(real_path, path);

	//인자가 4개면 t옵션이다.
	if (argc == 4) {
		if (strcmp(argv[2], "-t")) {
			//t옵션의 입력된 시간을 간격으로 모니터링
			mn_time=atol(argv[3]);
		}

		if ((mn_time = atol(argv[3])) < 0) {
			printf("Usage:\n");
			printf("  > add <FILENAME> [OPTION]\n");
			printf("\t-t : Restart monitoring at <TIME> intervals\n");		
			return;
		}
	} else //t옵션이 아니면 그냥 모니터링 시간 1초
		mn_time = 1;

	init_dae=1;
	//디몬프로세스를 실행하는 함수입니다.
	init_daemon(real_path, mn_time);

	printf("monitoring started (%s)\n", real_path);
	return;
}
//프로세스를 삭제하는 함수입니다.
void execute_delete(int argc, char *argv[]) {
	char line[BUFLEN];
	char path[BUFLEN];
	char *tmpfile = "__tmp.txt";
	pid_t pid, dpid;
	FILE *fp, *fp2;

	//2개 인자가 안들어오면 delete 사용법을 출력합니다.
	if (argc != 2) {
		printf("Usage:\n");
		printf("  > delete <DAEMON_PID>\n");
		return;
	}
	//프로세스 번호가 0이하면 delete 사용법을 출력합니다.
	if ((pid = atoi(argv[1])) <= 0) {
		printf("Usage:\n");
		printf("  > delete <DAEMON_PID>\n");
		return;
	}

	int length=snprintf(NULL,0,"%d",atoi(argv[1]));
	char *data = malloc(length+1);
	int line_pid=atoi(argv[1]);
	snprintf(data, length+1, "%d", line_pid);
	
	//monitor_list파일을 엽니다.
	if ((fp = fopen(monitor_list, "r")) == NULL) {
		printf("Usage:\n");
		printf("  > delete <DAEMON_PID>\n");
		return;
	}
	//tmpfile을 엽니다.
	if ((fp2 = fopen(tmpfile, "w+")) == NULL) {
		printf("Usage:\n");
		printf("  > delete <DAEMON_PID>\n");
		return;		
	}

	fseek(fp, 0, SEEK_SET);
	bzero(line, BUFLEN);
	//monitor_list파일에서 한문장씩 읽어 옵니다.
	while (fgets(line, sizeof(line), fp)!=NULL){
		if(strstr(line, data)!=NULL)	//프로세스 번호가 포함 되어 있으면 건너뛴다.
			continue;
		//tmpfile파일에 붙여넣는다.
		fputs(line, fp2);		
	}
	//프로세스를 종료한다.
	if(kill(pid, SIGTERM)!=0){
		fprintf(stderr, "pid kill error\n");
		exit(1);
	}

	fclose(fp);
	fclose(fp2);
	remove(monitor_list);
	rename(tmpfile, monitor_list);

	return;
}

//tree를 실행시키는 함수입니다.
void execute_tree(int argc, char *argv[]){
	struct stat statbuf;
	char path[BUFLEN];
	char real_path[BUFLEN];
	char line[BUFLEN];
	FILE *fp;
	tree *head;
	int check = 0;
	
	//인자 두개가 안들어오면 tree사용법을 출력한다.
	if (argc != 2) {
		fprintf(stderr, "Usage : tree <DIRPATH>\n");
		return;
	}

	strcpy(path, argv[1]);

	//파일이 없으면 tree사용법 호출
	if (stat(path, &statbuf) < 0) {
		printf("file error\n");
		return;
	}
	//해당 파일이 폴더가 아니면 tree사용법 호출
	if (!S_ISDIR(statbuf.st_mode)) {
		printf("Usage : tree <DIRPATH>\n");
		return;
	}

	// '/'로 시작하지 않으면 절대경로로 만들어준다.
	if (path[0] != '/')
		realpath(path, real_path);
	else	//절대 경로이기 떄문에 real_path에 넣어준다.
		strcpy(real_path, path);		

	head = create_node(real_path, 0, 0, path);
	make_tree(&head, real_path);
	//출력함수입니다.
	print_tree(head);
	return;
}
//트리를 출력하는 함수입니다.
void print_tree(tree *cur){
	struct stat statbuf;
	tree* head = cur;
	if(head==NULL)
		return;
	while(head->next!=NULL){
		if(head->isEnd)
			printf("%s %s\n", tree_end,head->name);
		else printf("%s %s isDir:%d\n", tree_mid, head->name, head->isDir);
		
		/*if ((head->isDir)) {
			execute_tab(head->child);
		}*/
		
		head=head->next;
	}
}

void execute_tab(tree *head){
	tree* cur;
	cur = head;
	if(cur->next==NULL)
		return;
	while(cur->next!=NULL){		
		  printf("%s\t%s\n",tree_tab, cur->path);
		  cur=cur->next;
	}
}
//help를 실행시키는 함수
void execute_help(int argc, char *argv[]) {
	printf("Usage:\n");
	printf("  > add <FILENAME> [OPTION]\n");
	printf("\t-t : Restart monitoring at <TIME> intervals\n");
	printf("  > delete <DAEMON_PID>\n");
	printf("  > tree <DIRPATH>\n");
	printf("  > help\n");
	printf("  > exit\n");
}
//exit을 실행시키는 함수
void execute_exit(int argc, char *argv[]) {
	exit(0);
}
//디몬프로세스를 실행시킨다.
void init_daemon(char *dirpath, time_t mn_time) {
	pid_t pid, dpid;
	tree *head, *new_tree;
	char path[BUFLEN];
	FILE *fp;
	int fd,maxfd;

	sprintf(path, "%s/%s", dirpath, "log.txt");
	head = create_node(dirpath, 0, 0, "");
	make_tree2(&head, dirpath);


	if ((pid = fork()) < 0) {
		fprintf(stderr, "in function init_daemon: fork error\n");
		exit(1);
	}
	else if(pid!=0){
		if(init_dae){
			return;
		}
		exit(1);
	}
	else if (pid == 0) { //child
		setsid();
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		//signal(SIGINT, signal_handler);

		maxfd = getdtablesize();
		for (fd = 3; fd < maxfd; fd++) //for debug , fd=3
			close(fd);

		umask(0);	
		//	chdir("/");
		fd = open("/dev/null", O_RDWR);
		dup(0);
		dup(0);
	}
	else
		return;

	if(access(path, F_OK)<0){
		if((fd=open(path, O_RDWR|O_CREAT|O_TRUNC, 0666))<0){
			fprintf(stderr, "creat error\n");
			exit(1);
		}
	}
	else{
		if((fd=open(path, O_RDWR|O_APPEND, 0666))<0){
			fprintf(stderr, "creat error\n");
			exit(1);
		}
	}
	fp = fopen(monitor_list, "a+");
	fprintf(fp, "%s %d\n", dirpath, getpid());
	fclose(fp);

	//해당 모니터링하기 위해 시간 만큼 계속 반복해서 탐색한다.
	while(1){
		//바뀐게 있는지 확인하는 변수
		new_tree = create_node(dirpath, 0, 0, "");
		make_tree2(&new_tree, dirpath);
		//기존 노드랑 새로받은 노드가 달라진게 있는지 확인하는 함수입니다.
		compare_tree2(head, new_tree, dirpath, fd);
		free_tree(head);
		//free_tree(new_tree);
		head=new_tree;
		//받은 시간 만큼 sleep한다.
		sleep(mn_time);
	}
	close(fd);
}

//새로운 노드 만든다.	
tree *create_node(char *path, mode_t mode, time_t mtime, char *name) {
	tree *new;

	new = (tree *)malloc(sizeof(tree));
	strcpy(new->path, path);
	strcpy(new->name, name);
	new->isEnd = false;
	new->isDir = false;
	new->mode = mode;
	new->mtime = mtime;
	new->next = NULL;
	new->prev = NULL;
	new->child = NULL;
	new->parent = NULL;

	return new;
}
//새로 만든 디렉토리 노드에다가 데이터를 저장
void make_tree(tree **dir, char *path) {
	struct dirent **filelist;
	struct stat statbuf;
	char new_path[BUFLEN];
	int count;

	if ((count = scandir(path, &filelist, scandir_filter, alphasort)) < 0) {
		fprintf(stderr, "in function make_tree: scandir error for %s\n", path);
		return;
	}
	tree* last_node = NULL;
	for (int i = 0; i < count; i++) {
		tree *new_node;

		bzero(new_path, BUFLEN);
		sprintf(new_path, "%s/%s", path, filelist[i]->d_name);
		if (stat(new_path, &statbuf) < 0) {
			fprintf(stderr, "stat error\n");
			exit(1);
		}

		new_node = create_node(new_path, statbuf.st_mode, statbuf.st_mtime, filelist[i]->d_name);
	
		new_node->next = (*dir);
		if(*dir != NULL){
			(*dir)->prev = new_node;
		}
		(*dir) = new_node;
		
		if (i == count - 1){
			new_node->isEnd = true;
			last_node = new_node;
		}

		if (S_ISDIR(statbuf.st_mode)){
			new_node->isDir = true;
			make_tree(&(new_node->child), new_path);
		}
	}
	if(last_node != NULL){
		last_node->next = NULL;
	}
}
//모든 노드를 한번에 모아둔다.
void make_tree2(tree **dir, char *path) {
	struct dirent **filelist;
	struct stat statbuf;
	char new_path[BUFLEN];
	int count;

	if ((count = scandir(path, &filelist, scandir_filter, alphasort)) < 0) {
		fprintf(stderr, "in function make_tree: scandir error for %s\n", path);
		return;
	}
	//printf("count:%d\n", count);

	for (int i = 0; i < count; i++) {
		tree *new_node;

		bzero(new_path, BUFLEN);
		sprintf(new_path, "%s/%s", path, filelist[i]->d_name);

		if (stat(new_path, &statbuf) < 0) {
			fprintf(stderr, "stat error\n");
			exit(1);
		}

		new_node = create_node(new_path, statbuf.st_mode, statbuf.st_mtime, filelist[i]->d_name);
		//printf("new_node_path:%s\n",new_node->path);

		new_node->next = (*dir);
		(*dir) = new_node;


		if (i == count - 1)
			new_node->isEnd = true;

		if (S_ISDIR(statbuf.st_mode)){
			new_node->isDir = true;
			make_tree2(dir, new_path);
		}
	}	
}

//비교함수
void compare_tree2(tree* old_tree, tree *new_tree, char dirPath[],int fd){
	char path[PATHLEN];
	strcpy(path, dirPath);
	time_t curtime;
	struct tm* t;
	char daystr[PATHLEN];
	char message[BUFLEN];

	curtime = time(NULL);
	t = localtime(&curtime);

	//daystr을 PATHLEN만큼 0으로 초기화
	memset(daystr, 0, PATHLEN);
	//daystr을 현재시간 저장
	sprintf(daystr, "%d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, 
			t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	// 이전 상태의 트리와 새로운 상태의 트리를 순회하면서 비교
	tree *old_node = old_tree;
	//tree *new_node = new_tree;
	
	while(old_node->next!=NULL){
		int cmp=0;
		tree *new_node = new_tree;
		while(new_node->next!=NULL){
			cmp = strcmp(old_node->path, new_node->path);
			if (cmp == 0) {
				// 경로가 동일한 경우, 수정 여부 확인
				if (old_node->mtime != new_node->mtime) {
					write_log(message,daystr,"modify", new_node, fd);
				}
				break;
			}
			new_node=new_node->next;
		}
		if (cmp != 0) {
			// 이전 상태의 트리에만 존재하는 경우, 삭제된 항목
			write_log(message,daystr,"remove", old_node, fd);
		}
		/*else if(cmp!=0) {
			// 새로운 상태의 트리에만 존재하는 경우, 추가된 항목
			write_log(message,daystr,"create", new_node, fd);
		}*/
		old_node = old_node->next;
	}
	
	tree *new_node = new_tree;
	while(new_node->next!=NULL){
		int cmp=0;
		tree *old_node = old_tree;
		while(old_node->next!=NULL){
			cmp = strcmp(old_node->path, new_node->path);
			if (cmp == 0) {
				// 경로가 동일한 경우, 수정 여부 확인
				if (old_node->mtime != new_node->mtime) {
					//write_log(message,daystr,"modify", new_node, fd);
				}
				break;
			}
			old_node=old_node->next;
		}
		if(cmp!=0) {
			// 새로운 상태의 트리에만 존재하는 경우, 추가된 항목
			write_log(message,daystr,"create", new_node, fd);
		}
		new_node = new_node->next;
	}
}

//로그를 써주는 함수입니다.
void write_log(char message[], char daystr[], char *str, tree *data, int fd){	
	sprintf(message, "[%s][%s][%s]\n", daystr, str, data->path);
	
	if(!strcmp(str, "modify")){
		struct stat statbuf;
		if (stat(data->path, &statbuf) < 0) {
			fprintf(stderr, "stat error\n");
			exit(1);
		}
		if(!S_ISDIR(statbuf.st_mode))
			write(fd, message, strlen(message));
	}
	else
		write(fd, message, strlen(message));
}
//트리 초기화해주는함수입니다.
void free_tree(tree *cur) {
	if (cur->child != NULL)
		free_tree(cur->child);

	if (cur->next != NULL)
		free_tree(cur->next);

	if (cur != NULL) {
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child = NULL;
		free(cur);
	}
}

void signal_handler(int signum) {
	signal_received = 1;
}
//폴더 확인할때 필터를 해준다.
int scandir_filter(const struct dirent *file) {
	if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")
			|| !strcmp(file->d_name, "log.txt")
			|| !strcmp(file->d_name, monitor_list)) {
		return 0;
	}
	else
		return 1;
}
