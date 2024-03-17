#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_score.h"
#include "blank.h"

//문제점수 스코어 테이블
extern struct ssu_scoreTable score_table[QNUM];
//학생들의 아이디 테이블
extern char id_table[SNUM][10];
//c옵션에서 포함된 학생들 아이디 테이블
extern char c_table[SNUM][10];

//스코어 테이블
struct ssu_scoreTable score_table[QNUM];
//학생 id 테이블
char id_table[SNUM][10];
//c옵션 학생 id 테이블
char c_table[SNUM][10];
//err_node *errnode;

//학생들 디렉토리주소
char stuDir[BUFLEN];
//정답 디렉토리주소
char ansDir[BUFLEN];
//에러 디렉토리주소
char errorDir[BUFLEN];
//n옵션 주소
char newDir[BUFLEN];
//t옵션을 위한 이차원배열
char threadFiles[ARGNUM][FILELEN];
//i옵션을 위한 이차원배열
char iIDs[ARGNUM][FILELEN];
//c옵션을 위한 이차원배열
char cIDs[ARGNUM][FILELEN];

//n옵션이 포함 되어 있을때 true로 바뀐다
int nOption = false;
//e옵션일때 true로 바뀐다
int eOption = false;
//t옵션이 포함 되어있을때 true로 바뀐다
int tOption = false;
//m옵션이 포함 되어있을때 true로 바뀐다
int mOption = false;
//i옵션이 포함 되어있을때 true로 바뀐다
int iOption = false;
//c옵션이 포함 되어있을때 true로 바뀐다
int cOption = false;
//p옵션이 포함 되어있을때 true로 바뀐다
int pOption = false;
//인자가 있는지 확인
int checkargv = false;


//socre메인함수
void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	//받는 인자가 -h가 포함 되어있으면 옵션 사용 방법을 설명 해줍니다.
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}

	//메모리 초기화 해줍니다.
	memset(saved_path, 0, BUFLEN);
	if(argc >= 3 && strcmp(argv[1], "-i") != 0){
		strcpy(stuDir, argv[1]);
		strcpy(ansDir, argv[2]);
	}

	//옵션을 체크합니다. 만약 옵션에 맞는 사용법을 사용하지 않으면 사용법을 설명 해줍니다.
	if(!check_option(argc, argv))
		exit(1);

	//return;

	//처음에 작업했던 디렉토리를 복사해줍니다.
	getcwd(saved_path, BUFLEN);

	//학생 디렉토리가 있는지 체크해줍니다.
	if(chdir(stuDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}

	//학생 디렉토리로 작업을 바꿔줍니다.
	getcwd(stuDir, BUFLEN);

	chdir(saved_path);

	//정답 디렉토리가 있는지 체크해줍니다.
	if(chdir(ansDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}

	//정답 디레토리로 작업을 바꿔줍니다.
	getcwd(ansDir, BUFLEN);

	chdir(saved_path);

	//스코어 테이블 셋팅
	set_scoreTable(ansDir);
	//학생 id테이블 셋팅
	set_idTable(stuDir);

	/*for(int i=0; i<10; i++){
		printf("%d. %s\n", i, c_table[i]);
	}
	return;*/

	//m옵션이 ture이면 m옵션 실행
	if(mOption)
		do_mOption();

	printf("grading student's test papers..\n");
	score_students();
	
	//if(cOption)
		//do_cOption();
	
	return;
}

int check_option(int argc, char *argv[])
{
	int i, j, k;
	int c;
	int exist = 0;

	//getopt함수를 이용해서 옵션들을 비교해줍니다.
	while((c = getopt(argc, argv, "n:e:thmicp")) != -1)
	{
		switch(c){
			case 'n':
				nOption = true;
				strcpy(newDir, optarg);
				if(strstr(newDir, ".csv") == NULL)
				{
					printf("not correct .csv\n");
					exit(1);
				}
				break;
			case 'e':
				eOption = true;
				//errorDir에 주소값을 넣어줍니다.
				char err_d[FILELEN];
				strcpy(err_d, optarg);
				realpath(err_d, errorDir);
				//strcpy(errorDir, optarg);

				//errorDir이 접근이 불가능한지 확인합니다.
				if(access(errorDir, F_OK) < 0)
					//접근이 가능하면 errorDir에 주소값에 디렉토리를 생성합니다.
					mkdir(errorDir, 0755);
				else{
					//접근이 불가능 하면 해당 디렉토리를 삭제합니다.
					rmdirs(errorDir);
					//삭제 후 디렉토리를 생성합니다.
					mkdir(errorDir, 0755);
				}
				break;
			case 't':
				tOption = true;
				i = optind;	//i를 1로 초기화 해줍니다.
				j = 0;

				//옵션 인자가 없을때 까지 while문을 돌립니다.
				while(i < argc && argv[i][0] != '-'){
					//마지막 인자 전까지 threadFile배열에 데이터를 넣어줍니다.
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						strcpy(threadFiles[j], argv[i]);
					}
					i++; 
					j++;
				}

				//printf("%ld\n", strlen(threadFiles));
				break;
			case 'm':
				//m옵션 true로 변경
				mOption = true;
				break;
			case 'c':
				//c옵션 true로 변경
				cOption = true;
				i = optind;
				j = 0;
				//cIDs에 학번을 담아줍니다.
				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						checkargv = true;
						strcpy(cIDs[j], argv[i]);
					}
					i++;
					j++;
				}
				break;
			case 'p':
				//p옵션 true로 변경
				pOption = true;
				i = optind;
				j = 0;
				//cIDs에 학번을 담아줍니다.
				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						checkargv = true;
						strcpy(cIDs[j], argv[i]);
					}
					i++;
					j++;
				}
				break;
			case 'i':
				iOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){
					//마지막 인자전까지 iIDs배열에 데이터를 넣어줍니다.
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded. :: %s\n", argv[i]);
					else
						strcpy(iIDs[j], argv[i]);
					i++;
					j++;
				}
				break;
			case '?':
				//올바른 인자를 사용하지 않으면 사용법을 설명해줍니다.
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

//명세없어 사용하지 않는 함수
void do_iOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;

	//score.csv파일이 있는지 체크합니다.
	if((fp = fopen("./score.csv", "r")) == NULL){
		fprintf(stderr, "score.csv file doesn't exist\n");
		return;
	}

	// get qnames
	i = 0;
	//테이블의 첫출을 읽어옵니다.
	fscanf(fp, "%s\n", tmp);
	//첫 줄 첫번째 값을 생략해서 gname 배열에 첫번째 데이터를 넣어줍니다.
	strcpy(qname[i++], strtok(tmp, ","));
	
	//문자열 마지막까지 ","을 기준으로 잘라줍니다.
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);		//차레대로 gname 배열에 데이터를 넣어줍니다.

	// print result
	i = 0;		//i를 초기화 시켜 줍니다.

	while(i++ <= ARGNUM - 1)
	{
		exist = 0;
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%s\n", tmp);


		while(fscanf(fp, "%s\n", tmp) != EOF){
			//
			id = strtok(tmp, ",");

			if(!strcmp(ids[i - 1], id)){
				exist = 1;
				j = 0;
				first = 0;
				while((p = strtok(NULL, ",")) != NULL){
					if(atof(p) == 0){
						if(!first){
							printf("%s's wrong answer :\n", id);
							first = 1;
						}
						if(strcmp(qname[j], "sum"))
							printf("%s    ", qname[j]);
					}
					j++;
				}
				printf("\n");
			}
		}

		if(!exist)
			printf("%s doesn't exist!\n", ids[i - 1]);
	}

	fclose(fp);
}

//특정 문제에 배점을 바꿔주는 m옵션을 실행시키는 함수입니다.
void do_mOption(char *ansDir)
{
	//새로운 배점
	double newScore;
	char modiName[FILELEN];
	char filename[FILELEN];
	char *ptr;
	int i;

	ptr = malloc(sizeof(char) * FILELEN);


	while(1){
		//해당 문제 번호 받기
		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		//해당 번호에 no를 입력하면 배점 재입력 중단
		if(strcmp(modiName, "no") == 0)
			break;

		//테이블 최대크기만큼 스코어 테이블에 입력받은 값이 있는지 체크
		for(i=0; i < sizeof(score_table) / sizeof(score_table[0]); i++){
			//ptr에 테이블의 문제 번호를 저장합니다.
			strcpy(ptr, score_table[i].qname);
			ptr = strtok(ptr, ".");
			//저장된 문제 번호와 입력한 문제 번호가 같은지 체크
			if(!strcmp(ptr, modiName)){
				//같으면 새로운 배점을 입력받고 스코어 테이블 구조체  배열에 배점을 변경해준다.
				printf("Current score : %.2f\n", score_table[i].score);
				printf("New score : ");
				scanf("%lf", &newScore);
				getchar();
				score_table[i].score = newScore;
				break;
			}
		}
	}

	//filename에 ./score_table.csv를 저장하여 write_scoreTable함수를 이용해서 스코어 테이블을 변경 시켜줍니다.
	sprintf(filename, "./%s", "score_table.csv");
	write_scoreTable(filename);
	free(ptr);
}

//에러노드 추가해주는 함수
void insert(err_node ** root, char *qname, double score)
{
	//현재노드를 입력받은 노드를 설정해준다.
	err_node *cur=*root;
	//루트노드가 비어있다면 그곳에 정보를 채워준다.
	if(cur==NULL)
	{
		*root=(err_node *)malloc(sizeof(err_node));
		(*root)->score=score;
		strcpy((*root)->qname,qname);
		(*root)->next=NULL;
		return;
	}
	//루트노드가 비어있는게 아니라면 비워있는 노드까지 다음으로 옮긴다.
	while(cur->next!=NULL)
	{
		cur=cur->next;
	}
	//새로운노드에 입력받은 정보를 넣어준다.
	err_node * new_node=(err_node *)malloc(sizeof(err_node));
	new_node->score=score;
	strcpy(new_node->qname,qname);
	new_node->next=NULL;
	cur->next= new_node;
}

//d옵션 실행
void do_pOption(err_node** cur, char *qname, double score){
	//err_tok에 확장자 뺀 파일 명들을 넣어주는 위해 쓰는 토큰
	char *token;
	char err_tok[100];
	char err_qname[100];

	//확장자가 포함한 파일명 저장
	strcpy(err_tok, qname);
	
	//맨뒤 확장자 저장 
	token = strpbrk(err_tok, ".");
	//맨뒤 확장자를 null로 만들어서 파일명만 남김
	*token = '\0';
	
	//파일명을 err_qname에 저장
	strcpy(err_qname, err_tok);
	
	//노드에 파일명과 점수를 추가
	insert(cur, err_qname, score);
}

//노드 할당 초기
void node_reset(err_node* e_node){
	while(e_node != NULL){
		err_node* reset = e_node;
		e_node = e_node->next;
		free(reset);
		reset=NULL;
	}
}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}

//스코어 테이블을 설정해주는 함수
void set_scoreTable(char *ansDir)
{
	//스쿼어 테이블의 파일이름
	char filename[FILELEN];

	//스코어 테이블 파일이름 저장 ./score_table.csv
	sprintf(filename, "./%s", "score_table.csv");

	//스코어 테이블이 있는지 체크
	if(access(filename, F_OK) == 0)
		//스코어 테이블이 있으면 읽기로 불러들어옵니다.
		read_scoreTable(filename);
	else{
		//없으면 스코어 테이블을 만들어줍니다.
		make_scoreTable(ansDir);
		//스코어 테이블 생성 후 작성해줍니다.
		write_scoreTable(filename);
	}
}

//스코어 테이블의 데이터를 읽어 불러오는 함수
void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	//스코어 테이블이 읽기 권한이 있는지 체크합니다.
	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	//스코어 테이블이 마지막 데이터까지 읽어 들어옴
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		//score_table구조체 배열에 첫번째 배열부터 qname에 데이터를 저장시켜 줍니다.
		//마지막까지 데이터를 저장시켜 줍니다.
		strcpy(score_table[idx].qname, qname);
		//score_table구조체 배열에 첫번째 부터 score을 double로 변환하여 저장시켜 줍니다.
		//마지막까지 데이터를 저장시켜 줍니다.
		score_table[idx++].score = atof(score);
	}

	//스코어 테이블을 닫아줍니다.
	fclose(fp);
}

//스코어 테이블이 없을 경우 테이블을 만들어 주는 함수입니다.
void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp, *c_dirp;
	DIR *dp, *c_dp;
	char *tmp;
	int idx = 0;
	int i;

	//어떤 타입으로 스코어 테이블을 만들지 정하는 함수
	num = get_create_type();

	//첫번째 타입을 골랐을 경우
	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}

	//정답 디렉토리가 없으면 오류
	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	//디렉토리에 있는 파일 하나씩 불러읽어 옵니다.
	while((dirp = readdir(dp)) != NULL){
		//.이나 ..이 포함 되어있으면 넘어갑니다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		//타입이 -1로 리턴 되었다면 넘어갑니다.
		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		//스코어 테이블 구조체 배열에 파일 이름을 넣어줍니다.
		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	//해당 디렉토리를 닫습니다.
	closedir(dp);
	sort_scoreTable(idx);

	for(i = 0; i < idx; i++)
	{
		//파일의 타입을 type에 저장시켜 줍니다.
		type = get_file_type(score_table[i].qname);

		//위에서 저장된 num이 1일때
		if(num == 1)
		{
			//파일의 타입을 조건으로 score에 점수를 부여한다. 
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2)
		{
			//스코어 테이블에서 각각의 문제  데이터 마다 점수를 부여한다.
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}
		//부여한 점수를 스코어 테이블에 저장시킨다.
		score_table[i].score = score;
	}
}

//계산된 점수를 스코어 테이블에 입력해 주는 함수입니다.
void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	//테이블의 최대갯수
	int num = sizeof(score_table) / sizeof(score_table[0]);

	//스코어 테이블을 권한 666으로 생성 이미 있으면 오류
	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	//스코어 테이블을 전체적으로 작성
	for(i = 0; i < num; i++)
	{
		//점수가 0이면 break
		if(score_table[i].score == 0)
			break;

		//문제이름과 거기에 해당하는 점수를 작성 
		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		//파일에 tmp를 작성
		write(fd, tmp, strlen(tmp));
	}

	//파일 작성 후 해당 파일을 닫는다
	close(fd);
}

//학생디렉토리를 입력받아 학생아이디 테이블을 셋팅해주는 함수입니다.
void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;
	int cObject = 0;

	//학생디렉토리를 열수있는지 체크합니다.
	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}
	int c_check = 0;
	//학생디렉토리에 있는 파일들을 각각 하나씩 불러옵니다.
	while((dirp = readdir(dp)) != NULL){
		//.혹은 ..이면 건너뜁니다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		//디렉토리에 있는 파일명을 temp에 저장합니다.
		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		//파일이 디렉토리인지 확인합니다.
		if(S_ISDIR(statbuf.st_mode)){
			if((cOption || pOption)&&checkargv){
				for(int check=0; check<sizeof(cIDs)/sizeof(cIDs[0]); check++){
					if(!strcmp(dirp->d_name, cIDs[check])){
						strcpy(c_table[c_check], dirp->d_name);
						c_check++;
						break;
					}
				}
				strcpy(id_table[num], dirp->d_name);
				num++;
			}
			else
				//받아온 파일명이 디렉토리라면 파일명을 학생 아이디 테이블에 저장합니다.
				strcpy(id_table[num++], dirp->d_name);
		}
		else
			continue;
	}
	//학생 아이디 테이블을 저장했으면 학생 디렉토리를 닫아줍니다.
	closedir(dp);

	//학생 아이디 테이블을 정렬합니다.
	sort_idTable(num, c_check);
}

//학생 아이디 테이블을 정렬해주는 함수입니다.
void sort_idTable(int size, int c_size)
{
	int i, j;
	char tmp[10];

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			//두개의 아이디를 비교했을 때 더 크면 두 아이디를 스왑해줍니다.
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}

	if(checkargv){
		for(i = 0; i < c_size - 1; i++){
			for(j = 0; j < c_size - 1 -i; j++){
				//두개의 아이디를 비교했을 때 더 크면 두 아이디를 스왑해줍니다.
				if(strcmp(c_table[j], c_table[j+1]) > 0){
					strcpy(tmp, c_table[j]);
					strcpy(c_table[j], c_table[j+1]);
					strcpy(c_table[j+1], tmp);
				}
			}
		}
	}
}

//스코어 테이블 정렬하는 함수
void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){
			//스코어 테이블 구조체 배열에 j번째 qname을 가져옵니다.
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			//스코어 테이블 구조체 배열에 j+1번째 qname을 가져옵니다.
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			//비교할 데이터가 앞에 자리가 더 크거나 앞 자리는 같은데 뒷 자리가 더 크면 스왑해줍니다
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){
				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

//스코어 테이블에 qname을 가져오는 함수
void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	//dup배열에 qname을 저장합니다.
	strncpy(dup, qname, strlen(qname));

	//-.로 기준으로 맨처음 데이터를 인트형으로 변환하여 넣어줍니다.
	*num1 = atoi(strtok(dup, "-."));

	p = strtok(NULL, "-.");
	if(p == NULL)
		*num2 = 0;
	else
		*num2 = atoi(p);
}

//어떤 타입으로 스코어 테이블 만들지 정하는 함수
int get_create_type()
{
	int num;

	while(1)
	{
		//타입을 선택합니다.
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		//1또는 2가 아닌 다른 타입을 선택하면 오류를 출력합니다.
		if(num != 1 && num != 2)
			printf("not correct number!\n");
		else
			break;
	}

	//선택한 타입을 리턴해줍니다.(1 또는 2)
	return num;
}

//학생 점수를 매기는 함수입니다.
void score_students()
{
	//학생 점수
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	char csvDir[BUFLEN];
	//n옵션이 아니면 정답 디렉토리에 csv파일 만들기
	if(!nOption)
		strcpy(csvDir, ansDir);
	//n옵션이면 해당 디렉토리에 csv파일 만들기
	else
		strcpy(csvDir, newDir); 
	char *test=csvDir;
	if(!nOption){
		strcat(test, "/score.csv");
	}
	//학생 id 테이블 최대 크기
	int size = sizeof(id_table) / sizeof(id_table[0]);

	//666권한으로 score.csv를 생성합니다.
	if((fd = creat(test, 0666)) < 0){
		fprintf(stderr, "creat error for score.csv");
		return;
	}

	//첫번째 row를 작성합니다.
	write_first_row(fd);

	int c_check=0;
	for(num = 0; num < size; num++)
	{
		//학생 id테이블이 공백이면 종료
		if(!strcmp(id_table[num], ""))
			break;

		//학생 id테이블을 tmp에 저장
		sprintf(tmp, "%s,", id_table[num]);

		//학생 id 테이블을 스코어에 작성합니다.
		write(fd, tmp, strlen(tmp)); 

		//학생 합계 점수 계산
		if(cOption){
			if(checkargv){
				double c_score = score_student(fd, id_table[num]);
				if(!strcmp(c_table[c_check], id_table[num])){
					score += c_score;
					c_check++;
				}
			}
			else{
				score += score_student(fd, id_table[num]);
			}
		}
		else{
			score += score_student(fd, id_table[num]);
		}
	}

	//학생 명수에 총 점수를 빼서 평균을 구합니다.
	if(cOption){
		if(checkargv)
			printf("Total average : %.2f\n", score / c_check);
		else
			printf("Total average : %.2f\n", score / num);
	}

	printf("result saved.. (%s)\n", test);
	
	if(eOption){
		printf("error saved.. (%s)\n", errorDir);
	}

	close(fd);
}

int data_c_check = 0;

double score_student(int fd, char *id)
{
	//첫노드
	err_node *first = NULL;
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	for(i = 0; i < size ; i++)
	{
		//스코어 테이블 구조체 배열에 점수가 0점이면 종료
		//데이터가 없다는 말을 표현합니다.
		if(score_table[i].score == 0)
			break;

		//temp에 학생 디렉토리안에 학번에 답 경로를 저장해놓습니다.
		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		//해당 경로에 접근가능한지 체크합니다.
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			//접근 가능하면 type에 파일 확장자 타입을 받아옵니다.
			//해당 타입의 확장자가 .txt .c가 아닌 다른것이 오면 건너뜁니다.
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;

			//해당 타입에 해당하는 값을 리턴해줍니다.
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		//문제가 틀렸는지 확인합니다.
		if(result == false){
			//p옵션 실행
			if(pOption){
				do_pOption(&first, score_table[i].qname, score_table[i].score);
			}
			//틀렸으면 0점처리 합니다.
			write(fd, "0,", 2);
		}
		else{
			if(result == true){
				//맞았으면 스코어 테이블 구조체에서 점수를 더해줍니다.
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				//부분적으로 맞았으면 result의값을 더해주고서 점수를 줍니다.
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			//fd에 tmp를 작성해줍니다.
			write(fd, tmp, strlen(tmp));
		}
	}

	//p옵션이면 해당 노드에 wrong파일들 관리
	if(pOption){
		//c옵션이 포함되어 있으면 해당 파일에 점수 표시
		if(cOption){
			if(checkargv){
				if(!strcmp(c_table[data_c_check],id)){
					printf("%s is finished.. score : %.2f, ", id, score);
				}
				else
					printf("%s is finished.. \n", id);
			}
			else
				printf("%s is finished.. score : %.2f, ", id, score);
		}
		else
			printf("%s is finished.. \n",id);
		if(!checkargv){
			printf("wrong problem : ");
			//노드에 있는 데이터로 wrong problem 출력
			while(first != NULL){
				printf("%s(%.2lf)", first->qname, first->score);
				first = first->next;
				//다음 노드 데이터가 없으면 종료
				if(first == NULL)
					break;
				printf(", ");
			}
			printf("\n");
			first = NULL;
		}
		else{
			if(!strcmp(c_table[data_c_check], id)){
				data_c_check++;
				printf("wrong problem : ");
				//노드에 있는 데이터로 wrong problem 출력
				while(first != NULL){
					printf("%s(%.2lf)", first->qname, first->score);
					first = first->next;
					//다음 노드 데이터가 없으면 종료
					if(first == NULL)
						break;
					printf(", ");
				}
			printf("\n");
			first = NULL;
			}
			else{
				/*while(errnode != NULL){
					errnode = errnode->next;
					if(errnode == NULL)
						break;
				}*/
				node_reset(first);
			}
		}
	}
	//p옵션이 아니면 그냥 점수 출력
	else{
		if(cOption){
			if(checkargv){
				if(!strcmp(c_table[data_c_check],id)){
					printf("%s is finished.. score : %.2f\n", id, score);
					data_c_check++;
				}
				else
					printf("%s is finished.. \n", id);
			}
			else
				printf("%s is finished.. score : %.2f\n", id, score);
		}
		else
			printf("%s is finished.. \n", id); 
	}

	//tmp에 점수를 작성합니다.
	sprintf(tmp, "%.2f\n", score);
	//총점을 작성해줍니다.
	write(fd, tmp, strlen(tmp));

	return score;
}

//첫번째 row(열)를 작성하는 함수입니다.
void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	//스코어 테이블의 최대 크기
	int size = sizeof(score_table) / sizeof(score_table[0]);

	//맨 처음 칸은 빈칸으로 작성합니다.
	write(fd, ",", 1);

	for(i = 0; i < size; i++){
		//스코어가 0이면 종료합니다.
		//채점파일이 없다는 말이기도 합니다.
		if(score_table[i].score == 0)
			break;

		//tmp에 문제 번호를 저장합니다.
		sprintf(tmp, "%s,", score_table[i].qname);
		//해당 테이블에 문제번호를 입력합니다.
		write(fd, tmp, strlen(tmp));
	}
	//첫번째 열 맨 마지막 칸에는 sum을 입력해줍니다.
	write(fd, "sum\n", 4);
}

//정답을 받아오는  함수입니다.
char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	//result를 BUFLEN길이만큼 0으로 채워줍니다.
	memset(result, 0, BUFLEN);
	//fd에서 한글자씩 읽어 c에 저장합니다.
	//데이터가 NULL이면 종료
	while(read(fd, &c, 1) > 0)
	{
		//읽어온 글자가 :이면 종료
		if(c == ':')
			break;

		//한글자씩 result에 저장합니다.
		result[idx++] = c;
	}
	//result의 맨 뒤 글자가 \n인지 체크합니다.
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	//정답 result를 리턴해줍니다.
	return result;
}

//빈칸문제를 체점하는 함수입니다.
int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	//qname를 변수 크기만큼을 0으로 초기화 해줍니다.
	memset(qname, 0, sizeof(qname));
	//qname의 확장자의 길이를 뺀 만큼의 filename을 저장시켜줍니다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//tmp에 정답 학생디렉토리에 학번에 문제번호의 주소를 저장합니다.
	sprintf(tmp, "%s/%s/%s", stuDir, id, filename);
	//해당 tmp파일을 읽기전용 권한으로 열어줍니다.
	fd_std = open(tmp, O_RDONLY);
	//정답을 s_answer변수에 저장합니다.
	strcpy(s_answer, get_answer(fd_std, s_answer));

	//답이 공백이면 해당 디렉토리를 닫고 오류 반환
	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	//blank.c의 함수 check_brackets를 이용해서 ()를 체크합니다.
	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	//공백을 모두 지우지 말고 오른쪽 공백을 지우고 왼쪽 공백을 지워서 s_answer값을 변경합니다.
	strcpy(s_answer, ltrim(rtrim(s_answer)));

	//s_answer맨 마지막 글자가 ;인지 체크합니다.
	if(s_answer[strlen(s_answer) - 1] == ';'){
		//has_semicolon변수를 true로 변경합니다.
		has_semicolon = true;
		//s_answer 맨 마지막 글자를 ;에서 NULL로 변경해줍니다.
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	//blank.c의 함수 make_tokens를 이용해서 s_answer를 토큰화해줍니다.
	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	//blank.c의 트리 분석
	std_root = make_tree(std_root, tokens, &idx, 0);

	//정답 디렉토리의 문제파일 주소를 tmp에 저장합니다.
	sprintf(tmp, "%s/%s", ansDir, filename);
	//해당 파일 주소를 읽기전용 권한으로 열어줍니다.
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		//ans_root변수 초기화
		ans_root = NULL;
		result = true;

		//정답 토큰화하여 초기화시키는 작업입니다.
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		//읽어온 답을 a_answer변수에 저장합니다.
		strcpy(a_answer, get_answer(fd_ans, a_answer));

		//읽어온 답이 없으면 종료
		if(!strcmp(a_answer, ""))
			break;

		//읽어온 답을 왼쪽 오른쪽 공백을 제거하여 a_answer에 저장시킵니다.
		strcpy(a_answer, ltrim(rtrim(a_answer)));

		//위에서 s_answer로 마지막 글자가 ;를 체크하여 has_semicolon을 비교해줍니다.
		if(has_semicolon == false){
			//a_answer변수 마지막 글자가 ;이면 넘어가줍니다.
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		else if(has_semicolon == true)
		{
			//a_answer변수 마지막 글자가 ;이면 넘어가줍니다.
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			//그렇지 않으면 마지막 글자를 NULL을 넣어줍니다.
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		//토큰화에 실패했으면 다음 정답으로 넘어가줍니다.
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		//정답을 토큰 트리화 시켜줍니다. //blank.c make_tree 분석해야됨
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		//함수이름만 보고 학생 루트트리와 정답 루트 트리를 비교해서 맞으면 result에 true를 입력 하는 함수라고 예상 blank.c compare_tree분석 해야된다.
		compare_tree(std_root, ans_root, &result);


		if(result == true){
			close(fd_std);
			close(fd_ans);

			//노드 초기화를 해줍니다.
			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}

	close(fd_std);
	close(fd_ans);

	//노드 초기화 해줍니다.
	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

//c.프로그램 채점하는 함수입니다.
double score_program(char *id, char *filename)
{
	double compile;
	int result;

	//프로그램을 컴파일해줍니다.
	compile = compile_program(id, filename);

	//compile이 ERROR인지 체크합니다.
	if(compile == ERROR || compile == false)
		return false;

	//답을 비교해주는 해줘서 result에 넣어줍니다.
	result = execute_program(id, filename);

	if(!result)
		return false;

	if(compile < 0)
		return compile;

	return true;
}

//thread체크하는 함수입니다.
int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	if(strlen(threadFiles)<=0){
		return true;
	}

	for(i = 0; i < size; i++){
		//qname이 threadFile에 속하는지 반복문을 돌려서 판단합니다.
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

//프로그램을 컴파일 해주는 함수입니다.
double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	//qname변수를 0으로 초기화
	memset(qname, 0, sizeof(qname));
	//qname을 확장자전까지 길이만큼 filename을 저장시킵니다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//thread를 체크하여 해당 리턴 받은 값을 isthread변수에 넣어줍니다.
	isthread = is_thread(qname);

	//정답파일의 경로를 temp_f에 넣어줍니다.
	sprintf(tmp_f, "%s/%s", ansDir, filename);
	//컴파일해서 나올 정답 파일의 이름 경로를 tmp_e에 넣어줍니다.
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	//t옵션인지 확인 하여 정답경로를 컴파일 해줍니다.
	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	//정답의 에러파일의 경로를 temp_e에 넣었습니다.
	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);

	//해당 정답의 에러파일 권한을 666으로 생성합니다.
	fd = creat(tmp_e, 0666);

	//다시 컴파일하는 프로그램
	redirection(command, fd, STDERR);
	//에러파일의 사이즈를 담습니다.
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);//실행시켰던 파일을 닫아줍니다.

	//에러파일이 있으므로 에러 발생하여 false로 리턴해줍니다.
	if(size > 0)
		return false;

	//학생 디렉토리에 학번 디렉토리에 답안파일 경로를 tmp_f에 저장합니다.
	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	//학생 디렉토리에 학번 디레토리에 문제파일 경로를 tmp_e에 저장합니다.
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	//t옵션인지 확인합니다.
	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	//해당 에러 파일의 경로를 tmp_f에 담아줍니다.
	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	//해당 파일을 666권한으로 생성합니다.
	fd = creat(tmp_f, 0666);

	//컴파일을 한번 더 합니다.
	redirection(command, fd, STDERR);
	//fd=에러파일의 크기를 담아줍니다.
	size = lseek(fd, 0, SEEK_END);
	//에러 파일을 닫습니다.
	close(fd);

	//오류 파일이 있는지 체크
	if(size > 0){
		//옵션e인지 체크
		if(eOption)
		{
			//에러디렉토리 경로를 저장합니다.
			sprintf(tmp_e, "%s/%s", errorDir, id);
			//해당 경로가 접근 가능한지 체크
			if(access(tmp_e, F_OK) < 0)
				//755권한으로 해당 경로 디렉토리 생성
				mkdir(tmp_e, 0755);

			//에러 파일 경로를 저장합니다.
			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			//tmp_f를 tmp_e로 변경합니다.
			rename(tmp_f, tmp_e);

			//에러 체크하는 함수
			result = check_error_warning(tmp_e);
		}
		else{ 
			result = check_error_warning(tmp_f);
			//tmp_f를 닫습니다.
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

//에러 체크하는 함수
double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	//해당 에러파일을 읽기모드로 열어줍니다.
	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	//
	while(fscanf(fp, "%s", tmp) > 0){
		//tmp가 error:를 포함하는지 warning을 포함하는지 체크해줍니다.
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	//warning이면 warning을 리턴해줍니다.
	return warning;
}

//프로그램 실행하는 함수입니다.
int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	//0으로 초기화시켜줍니다.
	memset(qname, 0, sizeof(qname));
	//해당 파일의 길이에서 확장자 길이를 뺀 길이만큼 filename의 데이터를 qname에 넣어줍니다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//정답 파일을 ans_fname에 저장합니다.
	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	//해당 파일을 666권한으로 생성합니다.
	fd = creat(ans_fname, 0666);

	//정답 실행 파일을 tmp에 저장합니다.
	sprintf(tmp, "%s/%s.exe", ansDir, qname);
	//해당 파일을 실행 시켜줍니다.
	redirection(tmp, fd, STDOUT);
	close(fd);

	//학생 파일 경로를 std_fname에 저장합니다.
	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	//해당 파일을 666 권한으로 생성합니다.
	fd = creat(std_fname, 0666);

	//학생 실행파일을 tmp에 저장합니다.
	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	//처음 시작 시간을 잽니다.
	start = time(NULL);
	redirection(tmp, fd, STDOUT);

	//해당 실행파일을 tmp에 저장합니다.
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		//마지막 시간을 받습니다.
		end = time(NULL);

		//ssu_score.h의 OVER(5)초 만큼 차이나면 강제 종료
		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	//학생 답과 정답을 비교하여 리턴해줍니다.
	return compare_resultfile(std_fname, ans_fname);
}

//백그라운드로 실행하는 함수 입니다.(새 프로세스)
pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;

	//0을 초기화 해줍니다.
	memset(tmp, 0, sizeof(tmp));
	//background.txt를 읽기 쓰기, 생성으로 666 권한으로 열어줍니다.
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	//command에 name이 들어간 프로세스 명령어를 저장합니다.
	sprintf(command, "ps | grep %s", name);
	//name 프로세스를 체크합니다.
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	//tmp값이 없을 때 background.txt를 닫고 종료합니다.
	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	//" "을 기준을 tmp의 첫번째 데이터를 int형으로 변환하여 pid에 저장합니다.
	pid = atoi(strtok(tmp, " "));
	close(fd);

	//background.txt를 닫습니다.
	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	//학생 답 파일을 읽기전용으로 오픈합니다.
	fd1 = open(file1, O_RDONLY);
	//정답 파일을 읽기전용으로 오픈합니다.
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		//학생 답파일을 읽어옵니다.
		while((len1 = read(fd1, &c1, 1)) > 0){
			//빈칸은 건너뜁니다.
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		//정답 파일을 읽어옵니다.
		while((len2 = read(fd2, &c2, 1)) > 0){
			//빈칸일 경우 건너뜁니다.
			if(c2 == ' ') 
				continue;
			else 
				break;
		}

		if(len1 == 0 && len2 == 0)
			break;

		//읽어온 파일을 소문자로 바꿔줍니다.
		to_lower_case(&c1);
		to_lower_case(&c2);

		//답이 틀렸는지 확인
		if(c1 != c2){
			//틀렸으면 열었던 파일을 닫습니다.
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

//다시 컴파일러 해주는 함수
void redirection(char *command, int new, int old)
{
	int saved;

	//old를 saved에 복사합니다.
	saved = dup(old);
	//new를 old에 복재합니다.
	dup2(new, old);

	//다시 컴파일 해줍니다.
	system(command);

	//saved를 old에 복재합니다.
	dup2(saved, old);
	//saved를 닫습니다.
	close(saved);
}

//파일의 타입을 가져옵니다.
int get_file_type(char *filename)
{
	//.뒤에 있는 확장자를 저장합니다.
	char *extension = strrchr(filename, '.');

	//extension변수에 .txt가 포함 되어있으면 TEXTFILE(int형)을 리턴해준다.
	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	//extension변수에 .c가 포함 되어있으면 CFILE(int형)을 리턴해준다.
	else if (!strcmp(extension, ".c"))
		return CFILE;
	//그렇지 않은 .zip이나 다른 .확장자가 포함되어있으면 -1을 리턴해준다.
	else
		return -1;
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[50];

	if((dp = opendir(path)) == NULL)
		return;

	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name);

		if(lstat(tmp, &statbuf) == -1)
			continue;

		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		else
			unlink(tmp);
	}

	closedir(dp);
	rmdir(path);
}

//대문자를 소문자로 변경하는 함수
void to_lower_case(char *c)
{
	//대문자인지 확인합니다.
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

//옵션에 맞게 설명해주는 함수입니다.
void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -m                modify question's score\n");
	printf(" -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>       compile QNAME.C with -lpthread option\n");
	printf(" -t <IDS>          print ID's wrong questions\n");
	printf(" -h                print usage\n");
}
