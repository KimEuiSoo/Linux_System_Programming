#include <stdio.h>
#include <fcntl.h>
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
#define BUFSIZE 1024*16

//노드 구조체
typedef struct _node {
	char path[4000];
	struct _node *next;
} node;

//단일파일 복구 할 위치 폴더 추가하는 함수
void single_create_command(char* path);
//단일파일 복구하는 함수
void single_recover(char *argv);
//recover할 파일들의 폴더 추가하는 함수
void create_dir(void);
//디렉토리에 있는 파일 List에 넣어주는 함수
void List_Update(char* path);
//옵션 n 실행 함수
void N_flag(void);
//옵션 d 실행 함수
void D_flag(char* path);
//옵션 d,n 실행 함수
void D_N_flag(char* path);
//변수에 보관할 데이터를 넣기 위한 함수
void data_contain_fun(char* path);
//같은 이름의 파일이 몇개 있는지 세기 위한 함수
void compair_fun(char* path, int index);
//디렉토리 주소 만드는 함수
void D_create_command(char* path);
//파일복사하는 함수
int file_copy_command(void);
//천단위 컴마 찍는 함수
char* comma_command(long num);
//recover_filename받아오는 함수
void recover_filename_create_command(char* path);
//recover_path를 업데이트하는 함수
void recover_path_update(char* path);
//맨뒤에 이름 빼오는 함수
char* last_token_data(char* path);
//n옵션 recover_filename 받아오는 함수
void recover_new_filename_create_command(char* path);
//n옵션 recover_path를 업데이트하는 함수
void recover_new_path_update(char* path);

//현재위치한 디렉토리 주소
static char* current_path;
//복구파일이 위치할 주소
static char recover_path[4000];
//복구파일이 위치할 주소+파일명
static char recover_filename[4000];
//백업파일이 위치한 주소+파일명
static char backup_filename[4000];
//백업파일이 위치한 주소
static char backup_path[4000];
//파일 마지막 이름
static char file_lastname[4000];
//메인 노드의 헤드
node* main_head;
//추가 노드의 헤드
node* down_head;
//리스트 담는 변수
char* node_data[4000];
//n옵션 확인하는 변수
int n_flag;
//d옵션 확인하는 변수
int d_flag;
//n:이름 확인하는 변수
char n_name[4000];
//받은 인자 디렉토리 이름
char dir_argv[4000];

//노드를 추가해주는 함수
void insert(node** head, char* path){
	node* new_node = (node*)malloc(sizeof(node));
	strcpy(new_node->path, path);
	new_node->next = (*head);
	(*head) = new_node;
}

void List_Update(char* path){
	DIR* dir;
	struct dirent* ent;
	node* head;

	dir = opendir(path);

	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0){
				insert(&head, ent->d_name);
			}
		}
	}
	else{
		//printf("Error open\n");
		create_dir();
		return;
	}

	closedir(dir);

	main_head = head;

	/*while(main_head != NULL){
	  printf("%s\n", main_head->path);a
	  main_head = main_head->next;
	}*/
}

void Data_List_insert(char* path){
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
				insert(&main_head, dir_path);
			}
		}
	}
	else{
		//printf("Error open\n");
		create_dir();
		return;
	}

	closedir(dir);
}

int main(int argc, char** argv){
	strcpy(backup_path, "");
	strcpy(backup_filename, "");
	strcpy(recover_path, "");
	strcpy(recover_filename, "");
	strcpy(file_lastname, "");
	//node_data = (char**)malloc(sizeof(char*));
	//free(node_data);

	if(argv[2] == NULL){
		printf("Usage : recover <FILENAME> [OPTION]\n");
		printf("-d : recover directory recursive\n");
		printf("-n <NEWNAME> : recover file with new name\n");
	}

	else if(argv[3] == NULL){
		single_recover(argv[2]);		
	}
	else{
		char* path;
		path = argv[2];
		strcpy(dir_argv,path);
		//받는 인자 옵션
		int tmp = 0;
		d_flag = 0;
		n_flag = 0;
		strcpy(n_name,"");

		optind = 1;
		opterr = 0;
		while((tmp=getopt(argc, argv, "dn:")) != -1){
			switch(tmp){
				case 'd':
					d_flag = 1;
					break;
				case 'n':
					n_flag = 1;
					strcpy(n_name, optarg);
					break;
				default:
					printf("Usage : recover <FILENAME> [OPTION]\n");
					printf("-d : recover directory recursive\n");
					printf("-n <NEWNAME> : recover file with new name\n");
					exit(1);
			}
		}

		if(d_flag==1 && n_flag==1){
			char r_filename[4000] = {'\0'};
			char last_path[4000] = {'\0'};
			if(strstr(path, "./")!=NULL){
				char buffer[BUFSIZ] = "\0";
				path = realpath(".", buffer);
				strcat(path, "/");
			}
			else if(strstr(path, "/")==NULL || path[0] != '/'){
				char buffer[BUFSIZ] = "\0";
				char* pathname;
				pathname = path;
				path = realpath(".", buffer);
				strcat(path, "/");
				strcat(path, pathname);
			}
			strcpy(last_path, path);
			strcpy(dir_argv, last_token_data(last_path));
			D_N_flag(path);
		}
		else if(d_flag==1){
			char r_filename[4000] = {'\0'};
			if(strstr(path, "./")!=NULL){
				char buffer[BUFSIZ] = "\0";
				path = realpath(".", buffer);
				strcat(path, "/");
			}
			else if(strstr(path, "/")==NULL || path[0] != '/'){
				char buffer[BUFSIZ] = "\0";
				char* pathname;
				pathname = path;
				path = realpath(".", buffer);
				strcat(path, "/");
				strcat(path, pathname);
			}
			D_flag(path);
		}
		else if(n_flag==1){
			char r_filename[4000] = {'\0'};
			if(strstr(path, "/")==NULL || path[0] != '/'){
				char buffer[BUFSIZ] = "\0";
				char* filename;
				filename = path;
				path = realpath(".", buffer);
				strcat(path, "/");
				strcat(path, filename);
			}
			single_create_command(path);
			strcat(r_filename, recover_path);
			strcat(r_filename, n_name);
			strcpy(recover_filename, r_filename);
			N_flag();
		}
	}
}

void D_flag(char* path){
	char dirpath[4000] = {'\0'}; strcpy(dirpath,path);
	D_create_command(dirpath);
	data_contain_fun(backup_path);
}

void D_N_flag(char* path){
	char dirpath[4000] = {'\0'}; strcpy(dirpath,path);
	D_create_command(dirpath);
	data_contain_fun(backup_path);
}

void data_contain_fun(char* path){
	Data_List_insert(path);
	while(down_head != NULL){
		struct stat fileStat;

		if(stat(down_head->path, &fileStat) < 0){
			break;
		}

		if(S_ISDIR(fileStat.st_mode)){
			char new_path[256] = {'\0'};
			strcat(new_path, down_head->path);
			down_head = down_head->next;
			data_contain_fun(new_path);
		}
		else{
		}
		if(down_head != NULL)
			down_head = down_head->next;
	}

	int list_index=0;
	while(main_head != NULL){
		struct stat fileStat;

		if(stat(main_head->path, &fileStat) < 0){
			break;
		}

		if(S_ISDIR(fileStat.st_mode)){
		}
		else{
			node_data[list_index] = main_head->path;
			if(main_head == NULL)
				break;
			list_index++;
		}
		main_head = main_head->next;
		if(main_head == NULL)
			break;
	}

	for(int i=0; i<list_index; i++){
		if(i==0){
			compair_fun(node_data[i], list_index);
		}
		else{
			if(strstr(node_data[i], backup_filename)==NULL){
				compair_fun(node_data[i], list_index);
			}
		}
	}
}

void Several_file(char* path, int index){
	char file_search[4000] = {'\0'};
	strcpy(file_search, path);
	int file_index=0;
	file_index = index;
	char* select_filename[2000]={'\0'};

	int index_type = 1;
	select_filename[0] = "exit";
	printf("0. exit\n");

	for(int i=0; i<file_index; i++){
		if(strstr(node_data[i],file_search)!=NULL){
			int fd;
			off_t fsize;
			char* data_token;// = (char*)malloc(sizeof(char));;
			free(data_token);
			char file[4000] = {'\0'};
			char file_extention[20]="\0";
			char filename_path[4000] = {'\0'};
			strcpy(file, node_data[i]);
			strcpy(filename_path, node_data[i]);

			select_filename[index_type] = filename_path;

			data_token = strtok(file, "_");
			while(data_token != NULL){
				strcpy(file_extention, data_token);
				data_token = strtok(NULL,"_");
			}

			fd = open(filename_path, O_RDWR);

			fsize = lseek(fd, 0, SEEK_END);			
			char* size_comma = comma_command(fsize);

			printf("%d. %s\t\t%sbyte\n", index_type, file_extention, size_comma);
			index_type++;
		}
	}
	printf("Choose file to recover\n>> ");
	int select_type;
	select_type = 0;
	scanf("%d",&select_type);
	if(select_type == 0){
		return;
	}
	else{
		int s=1;
		int already_file=1;
		strcpy(backup_filename, select_filename[select_type]);
		struct stat fileStat;

		if(stat(recover_filename, &fileStat) != 0){
			already_file=0;
		}
		file_copy_command();
		if(already_file==1){
			printf("\"%s\" backup recover to %s\n", backup_filename, recover_filename);
		}
		else{
			printf("\"%s\" backup file recover\n", backup_filename);
		}
	}

}

void compair_fun(char* path, int index){
	char file_search[4000] = {'\0'};
	int file_index = 0;
	file_index = index;

	char token_path[4000] = {'\0'};
	char time_path[4000] = {'\0'};
	char time_date[4000] = {'\0'};
	char* token;

	int tok_index = 0;
	char* time_token;
	free(time_token);

	strcpy(token_path, path);
	strcpy(time_path, path);
	time_token = strtok(time_path, "_");

	while(time_token != NULL){
		tok_index++;
		strcpy(time_date, time_token);
		time_token = strtok(NULL, "_");
	}

	token = strtok(token_path, "_");
	strcpy(file_search,"");
	int last_tok = 0;
	while(token != NULL){
		last_tok++;
		if(strstr(token, time_date)==NULL)
			strcat(file_search, token);
		if((tok_index-1) > last_tok)
			strcat(file_search, "_");
		token = strtok(NULL, "_");
	}

	if(n_flag!=1){
		recover_filename_create_command(file_search);
		recover_path_update(file_search);
	}
	else{
		recover_new_filename_create_command(file_search);
		recover_new_path_update(file_search);
	}

	int same_file=0;

	for(int i=0; i<file_index; i++){
		if(strstr(node_data[i], file_search)!=NULL){
			same_file++;
		}
	}

	if(same_file>1){
		printf("backup file list of %s\n",recover_filename);
		Several_file(file_search, file_index);
	}
	else{
		int already_file = 1;
		strcpy(backup_filename, path);
		struct stat fileStat;

		if(stat(recover_filename, &fileStat) != 0){
			already_file=0;
		}
		file_copy_command();
		if(already_file==1){
			printf("\"%s\" backup recover to %s\n", backup_filename, recover_filename);
		}
		else{
			printf("\"%s\" backup file recover\n", backup_filename);
		}
	}

	strcpy(backup_filename, file_search);
}


void N_flag(){
	char* select_filename[2000]={'\0'};
	printf("backup file list of %s\n",recover_filename);

	char* list_data = backup_path;

	List_Update(list_data);
	int index_type = 1;
	select_filename[0] = "exit";
	printf("0. exit\n");
	while(main_head != NULL){
		if(strstr(main_head->path, file_lastname)!=NULL){
			int fd;
			off_t fsize;

			char* token;
			char file[4000] = {'\0'};
			char file_extention[20]="\0";
			char filename_path[4000] = {'\0'};
			strcpy(file, main_head->path);
			strcpy(filename_path, backup_path);
			strcat(filename_path, main_head->path);

			select_filename[index_type] = filename_path;

			token = strtok(file, "_");
			while(token != NULL){
				strcpy(file_extention, token);
				token = strtok(NULL,"_");
			}

			fd = open(filename_path, O_RDWR);

			fsize = lseek(fd, 0, SEEK_END);			
			char* size_comma = comma_command(fsize);

			printf("%d. %s\t\t%sbyte\n", index_type, file_extention, size_comma);
			index_type++;
		}
		main_head = main_head->next;
	}
	printf("Choose file to recover\n>> ");
	int select_type;
	select_type = 0;
	scanf("%d",&select_type);
	if(select_type == 0){
		return;
	}
	else{
		int s=1;
		int already_file = 1;
		strcpy(backup_filename, select_filename[select_type]);
		struct stat fileStat;

		if(stat(recover_filename, &fileStat) != 0){
			already_file=0;
		}
		file_copy_command();
		if(already_file==1){
			printf("\"%s\" backup recover to %s\n", backup_filename, recover_filename);
		}
		else{
			printf("\"%s\" backup file recover\n", backup_filename);
		}
	}
}

void single_recover(char* argv){
	char* path;
	char bp[4000];
	path = argv;

	char* select_filename[]={'\0'};

	char* filename;
	if(strstr(path, "/")==NULL || path[0] != '/'){
		char buffer[BUFSIZ] = "\0";
		filename = path;
		path = realpath(".", buffer);
		strcat(path, "/");
		strcat(path, filename);
	}
	single_create_command(path);

	struct stat fileStat;

	if(S_ISDIR(fileStat.st_mode)){
		printf("%s is a directory file\n", bp);
		return;
	}
	else{
		printf("backup file list of %s\n",recover_filename);

		char* list_data = backup_path;

		List_Update(list_data);
		printf("0. exit\n");
		int index_type = 1;
		int select_type=0;
		select_filename[0] = "exit";
		while(main_head != NULL){
			if(strstr(main_head->path, file_lastname)!=NULL){
				int fd;
				off_t fsize;

				char* token;
				char file[4000] = {'\0'};
				char file_extention[20] = {'\0'};
				char filename_path[4000] = {'\0'};
				strcpy(file, main_head->path);
				strcpy(filename_path, backup_path);
				strcat(filename_path, main_head->path);

				select_filename[index_type] = filename_path;

				token = strtok(file, "_");
				while(token != NULL){
					strcpy(file_extention, token);
					token = strtok(NULL,"_");
				}

				fd = open(filename_path, O_RDONLY);

				fsize = lseek(fd, 0, SEEK_END);

				char* size_comma = comma_command(fsize);

				printf("%d. %s\t\t%sbyte\n", index_type, file_extention, size_comma);
				index_type++;
			}
			main_head = main_head->next;
		}
		printf("Choose file to recover\n>> ");
		scanf("%d",&select_type);
		if(select_type == 0)
			return;
		else{
			int s=1;
			int already_file = 1;
			while(1){
				if(s==select_type){
					strcpy(backup_filename, select_filename[s]);
					break;
				}
				s++;
			}
			struct stat fileStat;

			if(stat(recover_filename, &fileStat) != 0){
				already_file=0;
			}
			file_copy_command();
			if(already_file==1){
				printf("\"%s\" backup recover to %s\n", backup_filename, recover_filename);
			}
			else{
				printf("\"%s\" backup file recover\n", backup_filename);
			}
		}
	}
}

char* last_token_data(char* path){
	char last_path[4000]={'\0'};
	static char last_file[4000]={'\0'};
	char* last_token;
	last_token = strtok(path, "/");
	while(last_token != NULL){
		strcpy(last_file,last_token);
		last_token = strtok(NULL, "/");
	}
	return last_file;
}

void recover_filename_create_command(char* path){
	char remove[] = "backup";
	char token_path[4000]={'\0'};
	char result[4000]={'\0'};

	char* token;
	strcpy(token_path, path);

	token = strtok(token_path, "/");
	while(token != NULL){
		if(strcmp(token, remove)!=0){
			strcat(result,"/");
			strcat(result, token);
		}
		token = strtok(NULL, "/");
	}
	strcpy(recover_filename, result);
}

void recover_new_filename_create_command(char* path){
	char remove[] = "backup";
	char new_dir[40] = {'\0'};
	char token_path[4000]={'\0'};
	char result[4000]={'\0'};

	strcpy(new_dir, dir_argv);
	char* token;
	strcpy(token_path, path);

	int len1 = strlen(new_dir);

	token = strtok(token_path, "/");
	while(token != NULL){
		int len2 = strlen(token);
		if(strcmp(token, remove)!=0){
			if(len1!=len2 || strcmp(new_dir, token)!=0){
				strcat(result,"/");
				strcat(result, token);
			}
			else{
				strcat(result, "/");
				strcat(result, n_name);
			}
		}
		token = strtok(NULL, "/");
	}
	strcpy(recover_filename, result);
}

void D_create_command(char* path){
	char insert[] = "backup";
	char *user;
	user = getlogin();
	//recover_path에 연결
	char real_path[4000] = {'\0'};
	//backup_path에 연결
	char new_path[4000] = {'\0'};
	//recover_filename에 연결

	if(n_flag!=1){
		char token_path[4000] = {'\0'};
		strcpy(token_path, path);

		char* token;
		char token_filename[4000] = {'\0'};

		token = strtok(token_path, "/");

		strcat(real_path, "/");
		strcat(new_path, "/");

		while(token != NULL){
			strcat(new_path, token);
			strcat(real_path, token);

			if(strcmp(token, user)==0){
				strcat(new_path, "/");
				strcat(new_path, insert);
				strcat(new_path, "/");
				strcat(real_path, "/");
			}
			else{
				strcat(new_path, "/");
				strcat(real_path,"/");
			}
			token = strtok(NULL, "/");
		}

		strcat(recover_path, real_path);
		strcat(backup_path, new_path);
	}
	else{
		char last_path[4000] = {'\0'};
		char token_path[4000] = {'\0'};
		strcpy(token_path, path);
		strcpy(last_path, path);

		char* token;
		char last_filename[4000] = {'\0'};

		strcpy(last_filename, last_token_data(last_path));
		int len1 = strlen(last_filename);
		token = strtok(token_path, "/");

		strcat(real_path, "/");
		strcat(new_path, "/");

		while(token != NULL){
			int len2 = strlen(token);
			strcat(new_path, token);
			if(len1!=len2 && strcmp(last_filename, token)!=0)
				strcat(real_path, token);
			else
				strcat(real_path, n_name);

			if(strcmp(token, user)==0){
				strcat(new_path, "/");
				strcat(new_path, insert);
				strcat(new_path, "/");
				strcat(real_path, "/");
			}
			else{
				strcat(new_path, "/");
				strcat(real_path,"/");
			}
			token = strtok(NULL, "/");
		}

		strcat(recover_path, real_path);
		strcat(backup_path, new_path);
	}
	//printf("bp : %s | rp : %s \n", new_path, real_path);
}

void recover_path_update(char* path){
	char remove[] = "backup";
	char update_path[4000] = {'\0'};
	char token_path[4000] = {'\0'};
	char last_filename[4000] = {'\0'};
	char last_path[4000] = {'\0'};

	strcpy(token_path, path);
	strcpy(last_path, path);

	char* token;
	strcat(last_filename, last_token_data(last_path));
	int len1 = strlen(last_filename);
	int rlen = strlen(remove);
	token = strtok(token_path, "/");
	while(token != NULL){
		int len2 = strlen(token);
		if(len1!=len2 || strcmp(last_filename, token)!=0){
			if(len2!=rlen || strstr(remove, token)==NULL){
				strcat(update_path, "/");
				strcat(update_path, token);
			}
		}
		token = strtok(NULL, "/");
	}

	strcpy(recover_path, update_path);
}

void recover_new_path_update(char* path){
	char remove[] = "backup";
	char new_dir[40] = {'\0'};
	char update_path[4000] = {'\0'};
	char token_path[4000] = {'\0'};
	char last_filename[4000] = {'\0'};
	char last_path[4000] = {'\0'};

	strcpy(new_dir, dir_argv);
	strcpy(token_path, path);
	strcpy(last_path, path);

	char* token;
	strcat(last_filename, last_token_data(last_path));
	int len1 = strlen(last_filename);
	int new_len = strlen(new_dir);
	int rlen = strlen(remove);
	token = strtok(token_path, "/");
	while(token != NULL){
		int len2 = strlen(token);
		if(len1!=len2 || strcmp(last_filename, token)!=0){
			if(len2!=rlen || strstr(remove, token)==NULL){
				if(new_len!=len2 || strcmp(token, new_dir)!=0){
					strcat(update_path, "/");
					strcat(update_path, token);
				}
				else{
					strcat(update_path, "/");
					strcat(update_path, n_name);
				}
			}
		}
		token = strtok(NULL, "/");
	}

	strcpy(recover_path, update_path);
}

void single_create_command(char* path){
	char insert[] = "backup";
	char *user;
	user = getlogin();
	//recover_path에 연결
	char real_path[4000] = {'\0'};
	//backup_path에 연결
	char new_path[4000] = {'\0'};
	//recover_filename에 연결
	char recover_filePath[4000] = {'\0'};
	char token_path[4000] = {'\0'};

	strcat(recover_filePath, path);
	strcat(token_path, path);

	char* token;
	char token_filename[4000] = {'\0'};
	char last_filename[4000] = {'\0'};

	strcat(last_filename, last_token_data(path));

	token = strtok(token_path, "/");

	strcat(real_path, "/");
	strcat(new_path, "/");

	while(token != NULL){
		strcpy(token_filename, token);
		if(strstr(token_filename, last_filename) == NULL){
			strcat(new_path, token);
			strcat(real_path, token);
		}

		if(strcmp(token, user)==0){
			strcat(new_path, "/");
			strcat(new_path, insert);
			strcat(new_path, "/");
			strcat(real_path, "/");
		}
		else{
			if(strstr(token_filename, last_filename) == NULL){
				strcat(new_path, "/");
				strcat(real_path,"/");
			}
		}
		token = strtok(NULL, "/");
	}

	strcat(recover_path, real_path);
	strcat(backup_path, new_path);
	strcat(recover_filename, recover_filePath);
	strcat(file_lastname, token_filename);
	//printf("bp : %s | rp : %s | rf : %s\n", new_path, real_path, recover_filePath);
}

void create_dir(){
	char create_path[4000] = {'\0'};
	char data_change[4000] = {'\0'};
	strcpy(data_change, recover_path);
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

int file_copy_command() {
	FILE *backup_open = (FILE*)malloc(sizeof(FILE));
	FILE *recover_open = (FILE*)malloc(sizeof(FILE));
	char backup_file[4000]={'\0'}, recover_file[4000]={'\0'};
	char buffer[1024];
	int bytes;
	int already_file=1;

	strcpy(backup_file, backup_filename);
	strcpy(recover_file, recover_filename);

	backup_open = fopen(backup_file, "rb");
	recover_open = fopen(recover_file, "wb");
	if(backup_open==NULL || recover_open==NULL){
		create_dir();
		backup_open = fopen(backup_file, "rb");
		recover_open = fopen(recover_file, "wb");
		if(backup_open==NULL || recover_open==NULL){
			fclose(backup_open);
			fclose(recover_open);
			return -1;
		}
	}

	if(!strcmp(backup_file, recover_file)){
	}




	/*while((bytes == fread(buffer, 1, sizeof(buffer), backup_open)) > 0){
	  fwrite(buffer, 1, bytes, recover_open);
	  }*/

	while((bytes=fgetc(backup_open))!=EOF)
		fputc(bytes, recover_open);

	fclose(backup_open);
	fclose(recover_open);
	//return already_file;
}

char* comma_command(long num){
	static char comma_str[64];
	char str[64];
	int idx, len, comidx = 0, mode;

	sprintf(str, "%ld", num);
	len = strlen(str);
	mode = len % 3;

	for(idx = 0; idx < len; idx++){
		if(idx != 0 && (idx) % 3 == mode){
			comma_str[comidx++] = ',';
		}
		comma_str[comidx++] = str[idx];
	}

	comma_str[comidx] = 0x00;
	return comma_str;
}
