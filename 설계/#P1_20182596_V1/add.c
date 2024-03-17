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

//Add실행시키는 함수
void add_command(char* input);
//backup내 파일 위치 추가 함수
void new_dir_path_command(char* path);
//backup할 파일들의 폴더 추가하는 함수
void create_dir(void);
//디렉토리에 있는 파일 List에 넣어주는 함수
int List_Update(char* path);
//backup파일 추가하는 함수
void backup_file(void);
//Input 데이터를 비교하는 함수
int input_command(char*);
//md5 해쉬값 받아오는 함수
char* md5_hash_command(char* path, char* result);
//sha1 해쉬값 받아오는 함수
char* sha1_hash_command(char* path, char* result);
//해시값 비교
int hash_compair(void);
//재귀함수
void recursive_fun(char* path, int rec);

char *cuserid(char *buf);

//노드 구조체
typedef struct _node {
	char path[4000];
	struct _node *next;
} node;


//현재위치한 디렉토리 주소
static char* current_path;
//백업파일이 위치할 주소
static char backup_path[4000];
//파일이 위치한 주소+파일명
static char data_filename[4000];
//백업파일이 위치한 주소+파일명
static char backup_filename[4000];
//데이터 이전 주소
static char before_path[4000];
//md5를 사용할지 sha1을 사용할지 저장해둔 값
char* hash_data;
//메인 노드의 헤드
node* main_head;
//추가 노드의 헤드
node* down_head;

//노드를 추가해주는 함수
void insert(node** head, char* path){
	node* new_node = (node*)malloc(sizeof(node));
	strcpy(new_node->path, path);
	new_node->next = (*head);
	(*head) = new_node;
}

//이전 노드로 이동하는 함수
void previous_node(node* head, node* current){
	node* previous_d = head;

	while(previous_d->next != current){
		previous_d = previous_d->next;
	}

	printf("%s\n", previous_d->path);

	//return previous;
}

//단일 파일을 관리하는 연결 리스트
int List_Update(char* path){
	//int error_ = 0;
	DIR* dir;
	struct dirent* ent;
	node* head;

	//폴더가 있는지 확인
	dir = opendir(path);

	//없으면 폴더 생성
	if(dir == NULL){
		create_dir();
		return 0;
	}
	else{
		while((ent = readdir(dir)) != NULL){
			if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0){
				insert(&head, ent->d_name);
			}
		}
	}

	closedir(dir);

	main_head = head;
	return 1;
}

//폴더 포함 여러개의 데이터 관리하는 연결 리스트
int Data_List_insert(char* path){
	DIR* dir;
	struct dirent* ent;

	dir = opendir(path);

	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0){
				char dir_path[4000] = {'\0'};
				strcat(dir_path, path);
				strcat(dir_path, ent->d_name);
				struct stat fileStat;

				if(stat(dir_path, &fileStat) < 0){
					break;
				}

				if(S_ISDIR(fileStat.st_mode))
					strcat(dir_path, "/");
				insert(&down_head, dir_path);
			}
		}
	}
	else{
		create_dir();
		return 0;
	}

	closedir(dir);
	main_head = down_head;
	return 1;
}

int main(int argc, char** argv){
	char* data[3];
	char* hash;

	strcpy(backup_path,"");
	strcpy(backup_filename, "");
	strcpy(data_filename, "");

	for(int i=1; i<argc; i++){
		if(i != argc){
			data[i] = argv[i];
		}
		hash_data = argv[i];
	}

	if(argc == 3){
		printf("Usage : add <FILENAME> [OPTION]\n");
		printf("  -d : add directory recursive\n");
	}
	else if(argc == 4){
		char* path;
		path = data[2];

		if(strstr(path, "help") != NULL){
			printf("Usage : add <FILENAME> [OPTION]\n");
			printf("  -d : add directory recursive\n");
			return 0;
		}

		struct stat fileStat;

		if(stat(path, &fileStat) < 0){
			printf("%s can't be backuped\n",path);
			return 0;
		}

		//디렉토리인지 파일인지 검사
		//디렉토리일때
		if(S_ISDIR(fileStat.st_mode)){
			printf("%s is a directory file\n", path);
			return 0;
		}
		//파일일때
		else{
			if(strstr(path, "/")==NULL || path[0] != '/'){
				char buffer[BUFSIZ]="\0";
				char* filename;
				filename = path;
				path = realpath(".",buffer);
				strcat(path,"/");
				strcat(path, filename);
			}
		}

		//백업할 경로를 만들어 주는 함수
		new_dir_path_command(path);
		//백업경로를 담을 변수
		char* list_data = backup_path;
		//create_dir();

		//백업할 폴더의 리스트를 담을 함수
		List_Update(list_data);

		int result = hash_compair();
		if(result==1){
			backup_file();
			printf("%s backuped\n", backup_filename);
		}
		else{
			printf("%s is already backuped\n", backup_filename);
		}
	}
	else if(argc == 5){
		char* path;
		char* option;
		path = data[2];
		option = data[3];

		struct stat fileStat;

		if(stat(path, &fileStat) < 0){
			printf("%s can't be backuped\n",path);
			return 0;
		}
		if(strstr(option, "help")!=NULL || strstr(option, "-d")==NULL){
			printf("Usage : add <FILENAME> [OPTION]\n");
			printf("  -d : add directory recursive\n");
			return 0;
		}

		if(S_ISDIR(fileStat.st_mode)){
			if(strstr(path, "./")!=NULL){
				char buffer[BUFSIZ]="\0";
				path = realpath(".",buffer);
				strcat(path, "/");
			}
			else if(strstr(path, "/")==NULL || path[0] != '/'){
				char buffer[BUFSIZ]="\0";
				char filename[256] = "\0";
				strcpy(filename, path);
				path = realpath(".", buffer);
				strcat(path, "/");
				strcat(path, filename);
				strcat(path, "/");
			}
			else
				strcat(path, "/");
			recursive_fun(path, 0);
		}
		else{
			printf("%s is a file\n", path);
			return 0;
		}
	}
}

void add_command(char* input){
	
}

//재귀 함수 path = 주소, rec = 맨 처음에 리스트 업데이트 했는지
void recursive_fun(char* path, int rec){
	int data_rec = 1;
	if(rec == 0) Data_List_insert(path);
	else{
		Data_List_insert(path);
	}
	while(down_head != NULL){
		struct stat fileStat;

		if(stat(down_head->path, &fileStat) < 0){
			break;
		}

		if(S_ISDIR(fileStat.st_mode)){
			char new_path[4000] = {'\0'};
			strcat(new_path, down_head->path);
			down_head = down_head->next;
			recursive_fun(new_path, 1);
		}
		else{
			strcpy(backup_path,"");
			strcpy(backup_filename, "");
			strcpy(data_filename, "");
			//백업할 경로를 만들어 주는 함수
			new_dir_path_command(down_head->path);
			//백업경로를 담을 변수
			char* list_data = backup_path;

			//백업할 폴더의 리스트를 담을 함수
			int direrror=0;
			direrror = List_Update(list_data);

			if(direrror == 0){
				direrror = List_Update(list_data);
			}

			int result = hash_compair();
			if(result==1){
				backup_file();
				printf("%s backuped\n", backup_filename);
			}
			else{
				printf("%s is already backuped\n", backup_filename);
			}
		}
		if(down_head != NULL)
			down_head = down_head->next;
	}
}

char* current_time(){
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	static char d_t[20]={'\0'};

	strftime(d_t, 20, "_%Y%m%d%H%M%S", t);

	return d_t;
}

char* last_token_data(char* path){
	char last_path[4000]={'\0'};
	static char last_file[256]={'\0'};
	char* last_token;
	last_token = strtok(path, "/");
	while(last_token != NULL){
		strcpy(last_file,last_token);
		last_token = strtok(NULL, "/");
	}
	return last_file;
}

void new_dir_path_command(char* path){
	char insert[] = "backup";
	char new_path[4000]={'\0'};
	char real_path[4000]={'\0'};
	char back_path[4000]={'\0'};
	char token_path[4000]={'\0'};

	char *buf;
	buf = getlogin();

	char* token;
	char token_filename[256]={'\0'};
	char last_filename[256]={'\0'};

	strcat(real_path, path);
	strcat(token_path, path);
	strcat(last_filename,last_token_data(path));

	token = strtok(token_path, "/");

	strcat(back_path, "/");
	strcat(new_path,"/");

	//파일의 경로를 backup추가하여 바꾸는 코드
	while(token != NULL){
		strcpy(token_filename, token);
		if(strstr(token_filename, last_filename)==NULL){
			strcat(new_path, token);
			strcat(back_path, token);
		}

		if(strcmp(token, buf)==0){
			strcat(new_path, "/");
			strcat(new_path, insert);
			strcat(new_path,"/");
			strcat(back_path, "/");
			strcat(back_path, insert);
			strcat(back_path, "/");
		}
		else{
			if(strstr(token_filename, last_filename)==NULL){
				strcat(new_path, "/");
				strcat(back_path, "/");
			}

		}

		token = strtok(NULL, "/");
	}

	strcat(new_path, token_filename);
	strcat(new_path, current_time());

	strcat(backup_path, back_path);
	strcat(data_filename, real_path);
	strcat(backup_filename, new_path);
}

void create_dir(){
	char create_path[4000] = {'\0'};
	char data_change[4000] = {'\0'};
	strcpy(data_change, backup_path);
	char* token;

	token = strtok(data_change, "/");
	char *user;
	user = getlogin();

	strcpy(create_path, "/");
	while(token != NULL){
		struct stat st = {0};
		strcat(create_path, token);
		if(strstr(token, "home")!=NULL);
		else if(strstr(token, user)!=NULL);
		else if(strstr(token, "backup")!=NULL);
		else{
			if(stat(create_path, &st) == -1){
				if(mkdir(create_path,0766)== -1){
					perror("mkdir failed");
					return;
				}
			}

		}

		strcat(create_path, "/");

		if(strlen(create_path) >= sizeof(create_path)){
			fprintf(stderr, "c");
			return;
		}
		token = strtok(NULL, "/");
		if(token == NULL)
			break;
	}
}

void backup_file(){
	char* file_path = data_filename;
	char* new_path = backup_filename;

	/*pid_t pid;

	if((pid = fork())<0){
		printf("fork error\n");
		exit(1);
	}
	else if(pid == 0){
		execl("/bin/cp", "cp", file_path, new_path, NULL);
	}
	else wait(NULL);*/
	FILE *file_open = (FILE*)malloc(sizeof(FILE));
	FILE *new_open = (FILE*)malloc(sizeof(FILE));
	char file_file[4000]={'\0'}, new_file[4000]={'\0'};
	char buffer[1024];
	int bytes;

	strcpy(file_file, file_path);
	strcpy(new_file, new_path);

	file_open = fopen(file_file, "rb");
	new_open = fopen(new_file, "wb");
	if(file_open==NULL || new_open==NULL){
		create_dir();
		file_open = fopen(file_file, "rb");
		new_open = fopen(new_file, "wb");
		if(file_open==NULL || new_open==NULL){
			fclose(file_open);
			fclose(new_open);
			return;
		}
	}

	if(!strcmp(file_file, new_file)){

	}

	/*while((bytes == fread(buffer, 1, sizeof(buffer), file_open)) > 0){
		fwrite(buffer, 1, bytes, new_open);
	}*/
	while((bytes=fgetc(file_open))!=EOF)
		fputc(bytes, new_open);

	fclose(file_open);
	fclose(new_open);
}

int hash_compair(){
	int md5_max = 40;
	char md5_hash[md5_max];
	int sha1_max = 60;
	char sha1_hash[sha1_max];

	char main_Data[60] = "";
	int compair_Error = 0;

	//printf("%s\n", data_filename);

	if(hash_data == "md5"){
		char md_data[md5_max];
		strcpy(md_data,md5_hash_command(data_filename, md5_hash));
		//main_Data = md_data;
		strcpy(main_Data, md_data);
	}
	else{
		char sh_data[sha1_max];
		strcpy(sh_data, sha1_hash_command(data_filename, sha1_hash));
		//main_Data = sh_data;
		strcpy(main_Data, sh_data);
	}

	while(main_head != NULL){
		char* compair_Data="";
		char compair_path[4000] = {'\0'};
		strcat(compair_path, backup_path);
		strcat(compair_path, main_head->path);
		if(hash_data == "md5")
			compair_Data=md5_hash_command(compair_path, md5_hash);
		else
			compair_Data=sha1_hash_command(compair_path, sha1_hash);

		if(strcmp(main_Data,compair_Data)==0){
			compair_Error++;
			break;
		}

		main_head = main_head->next;


	}
	if(compair_Error==0)
		return 1;
	else
		return 0;
}

char* md5_hash_command(char *path, char *result){
	MD5_CTX c;
	int md5_max = 33;
	unsigned char md5_hash[MD5_DIGEST_LENGTH];
	int fd;
	int i;
	static unsigned char buffer[SHRT_MAX];

	FILE *f;
	f= fopen(path, "r");
	if(f == NULL){
		//printf("ERROR: fopen error for %s\n", path);
		return NULL;
	}

	fd = fileno(f);
	MD5_Init(&c);
	for(;;)
	{
		i=read(fd, buffer, SHRT_MAX);
		if(i<=0) break;
		MD5_Update(&c, buffer, (unsigned long)i);
	}
	MD5_Final(&(md5_hash[0]), &c);

	char temp[40] = "";
	char token[5] = "";
	for(int i=0; i<16; i++){
		sprintf(token, "%02x", md5_hash[i]);
		strcat(temp, token);
	}

	static char result_tmp[40] = "";
	strcpy(result_tmp, temp);

	printf("%s\n", result_tmp);
	return result_tmp;
}



char* sha1_hash_command(char* path, char* result){
	SHA_CTX c;
	int sha1_max = 60;
	unsigned char md5_hash[SHA_DIGEST_LENGTH];
	int fd;
	int i;
	static unsigned char buffer[SHRT_MAX];

	FILE *f;
	f= fopen(path, "r");
	if(f == NULL){
		//printf("ERROR: fopen error for %s\n", path);
		return NULL;
	}

	fd = fileno(f);
	SHA1_Init(&c);
	for(;;)
	{
		i=read(fd, buffer, SHRT_MAX);
		if(i<=0) break;
		SHA1_Update(&c, buffer, (unsigned long)i);
	}
	SHA1_Final(&(md5_hash[0]), &c);

	char temp[60] = "";
	char token[5];
	for(int i=0; i<16; i++){
		sprintf(token, "%02x", md5_hash[i]);
		strcat(temp, token);
	}

	static char result_tmp[60]="";
	strcpy(result_tmp, temp);

	return result_tmp;
}
