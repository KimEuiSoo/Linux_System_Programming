#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};


operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}	
	,{"*", 4}	,{"/", 3}	,{"%", 2}	
	,{"+", 6}	,{"-", 5}	
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};


//트리비교하는 함수이다.
void compare_tree(node *root1,  node *root2, int *result)
{
	node *tmp;
	int cnt1, cnt2;

	//정답 트리나 학생답 트리가 존재하지 않을 겨우에 종료한다.
	if(root1 == NULL || root2 == NULL){
		*result = false;
		return;
	}
	
	//root1이 비교 연산자일 경우를 체크한다.
	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){
		//두 트리가 루트의 값이 다른경우
		if(strcmp(root1->name, root2->name) != 0){

			if(!strncmp(root2->name, "<", 1))
				strncpy(root2->name, ">", 1);

			else if(!strncmp(root2->name, ">", 1))
				strncpy(root2->name, "<", 1);

			else if(!strncmp(root2->name, "<=", 2))
				strncpy(root2->name, ">=", 2);

			else if(!strncmp(root2->name, ">=", 2))
				strncpy(root2->name, "<=", 2);

			root2 = change_sibling(root2);
		}
	}
	//다시 비교했을때도 다르면 틀렸다고 판단하고 종료
	if(strcmp(root1->name, root2->name) != 0){
		*result = false;
		return;
	}
	//root1의 자식노드가 없는데 2는 있거나 root1의 자식노드는 있는데 2가 없는경우 일떄
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false;
		return;
	}
	
	//root1의 자식노드가 존재하면
	else if(root1->child_head != NULL){
		//root1의 자식노드와 2의 자식노드가 sibling 갯수가 다른경우에 결과값 false리턴 후 종료
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false;
			return;
		}
		//root1이 ==, !=일때
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{
			//root1의 자식노드와 root2의 자식노드를 비교한다.
			compare_tree(root1->child_head, root2->child_head, result);
			//
			if(*result == false)
			{
				*result = true;
				root2 = change_sibling(root2);
				compare_tree(root1->child_head, root2->child_head, result);
			}
		}
		//root1이 +, *, |, &, ||, && 연사자일때
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{
			//자식노드의 숫자가 다르면 false 리턴 후 종료한다.
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false;
				return;
			}
			//temp의 root2의 자식노드 저장
			tmp = root2->child_head;
			//첫번째 노드까지 계속 이전으로 만든다.
			while(tmp->prev != NULL)
				tmp = tmp->prev;
			
			while(tmp != NULL)
			{	
				//root1의 자식 노드와 트리를 비교한다.
				compare_tree(root1->child_head, tmp, result);
				//정답이면 true를 저장
				if(*result == true)
					break;
				else{
					//아니면 다음에도 계속 비교
					if(tmp->next != NULL)
						*result = true;
					tmp = tmp->next;
				}
			}
		}
		//그외의 경우일때 재귀호출 한다.
		else{
			compare_tree(root1->child_head, root2->child_head, result);
		}
	}	

	//root1의 다음노드가 존재하면
	if(root1->next != NULL){
		//다음노드의 수를 카운트 해서 root2의 카운트가 다르면 false 리턴 후 종료한다.
		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){
			*result = false;
			return;
		}
		//결과가 true일때
		if(*result == true)
		{
			//연산자를 얻기위해서 함수를 실행한다.
			tmp = get_operator(root1);
			//해당 얻은 연산자가 +, *, |, &, ||, &&중 하나일 경우
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{	
				//tmp에 roo2저장한다.
				tmp = root2;
				//tmp첫번째 노드까지 당겨준다
				while(tmp->prev != NULL)
					tmp = tmp->prev;
				
				while(tmp != NULL)
				{
					//root1과 tmp를 비교한다.
					compare_tree(root1->next, tmp, result);

					if(*result == true)
						break;
					else{
						if(tmp->next != NULL)
							*result = true;
						tmp = tmp->next;
					}
				}
			}
			//그외의 경우에는 다음 데이터들로 재귀호출한다.
			else
				compare_tree(root1->next, root2->next, result);
		}
	}
}

//토큰을 만들어주는 함수입니다.
int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN])
{
	//문자열 포인터를 저장하여 첫번쨰와 마지막을 만든다.
	char *start, *end;
	char tmp[BUFLEN];
	char str2[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	//토큰을 비워준다.
	clear_tokens(tokens);

	start = str;
	
	//str이 선언문도 아니고 gcc도 아니면 false를 리턴해준다.
	if(is_typeStatement(str) == 0) 
		return false;	
	
	while(1)
	{
		//연산자에 (),^문자들이 없으면 반복문 종료한다.
		if((end = strpbrk(start, op)) == NULL)
			break;

		//첫번째 문자열과 끝 일치할 경우
		if(start == end){
			//start가 ++, --인지 구분한다.
			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){
				//++++, ----면 false를 리턴한다.
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4))
					return false;

				// ex) ++a
				//연산자의 붙어있는 공백을 제거하고 is_character에 넘거서 실행한다.
				if(is_character(*ltrim(start + 2))){
					//row-1번쨰의 널문자 전 마지막 문자를 체크한다.
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; //row 0 이하면 false를 리턴한다.

					//++, --연산 후에 연산자를 	찾아서 저장한다.
					end = strpbrk(start + 2, op);
					//end가 널일경우에 str마지막으로 이동한다.
					if(end == NULL)
						end = &str[strlen(str)];
					//start가 end 앞에 있을 경우에 반복한다.
					while(start < end) {
						//start의 전 칸이 띄워쓰기이고 토큰의 row의 널문자 이전 문자가 널일 경우에 false를 return 해준다.
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;
						//띄워쓰기가 아니면 토큰의 로우 위치에 start 위치 한칸을 붙여넣어준다.
						else if(*start != ' ')
							strncat(tokens[row], start, 1);
						start++;	
					}
				}
				//row - 1의 마지막 문자가 null이 아니면
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					//row-1의 문자가 ++, --를 포함하면 false를 리턴한다.
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false;

					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row - 1], tmp);
					start += 2;
					row--;
				}
				//그 외의 경우
				else{
					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row], tmp);
					start += 2;
				}
			}
			//start의 대입연사자나 비교연산자가 존재하는지 체크한다.
			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){

				strncpy(tokens[row], start, 2);
				start += 2;
			}
			//start가 ->일 경우
			else if(!strncmp(start, "->", 2))
			{
				//end를 start다음칸으로 옮긴다.
				end = strpbrk(start + 2, op);

				if(end == NULL)
					end = &str[strlen(str)];

				while(start < end){
					if(*start != ' ')
						strncat(tokens[row - 1], start, 1);
					start++;
				}
				row--;
			}
			//end가 &일 경우에는
			else if(*end == '&')
			{
				//row가 0이나 row-1에 op를 포함한 경우
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){
					//start다음 위치에 op를 end에 저장
					end = strpbrk(start + 1, op);
					//end가 NULL이면 str 끝으로 설정
					if(end == NULL)
						end = &str[strlen(str)];
					
					//row에 start 다음에 삽입
					strncat(tokens[row], start, 1);
					start++;

					while(start < end){
						//start가 공백이거나 row의 마지막 문자가 &일때 false를 리턴한다.
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;
						else if(*start != ' ')
							strncat(tokens[row], start, 1);
						start++;
					}
				}
				//그 외의 경우
				else{
					//start 담음칸에 row를 저장한다.
					strncpy(tokens[row], start, 1);
					start += 1;
				}
				
			}
			//end가 *일 경우에
		  	else if(*end == '*')
			{
				isPointer=0;
				//row가 0보다 클때
				if(row > 0)
				{
					
					for(i = 0; i < DATATYPE_SIZE; i++) {
						//row-1에서 데이터 타입이 포함되어 있을때
						if(strstr(tokens[row - 1], datatype[i]) != NULL){
							//row-1에 *붙여넣는다
							strcat(tokens[row - 1], "*");
							start += 1;	
							isPointer = 1;
							break;
						}
					}
					//isPointer가 1일때
					if(isPointer == 1)
						continue;
					if(*(start+1) !=0)
						end = start + 1;

					//row가 1보다 크고 row-2가*이고 row-1 전부가 *일 경우
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						//row-1뒤에 start값을 end-start만큼을 붙여넣는다.
						strncat(tokens[row - 1], start, end - start);
						row--;
					}
					
					//row-1에 마지막 문자가 문자인 경우에
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						//row뒤에 start값을 end-start만큼을 붙여넣는다.
						strncat(tokens[row], start, end - start);   
					}

					// ex) ,*b (pointer)
					else if(strpbrk(tokens[row - 1], op) != NULL){		
						strncat(tokens[row] , start, end - start); 
							
					}
					else
						strncat(tokens[row], start, end - start);

					start += (end - start);
				}
				//row가 0일때
			 	else if(row == 0)
				{
					//start의 1칸뒤에 op가 없을경우
					if((end = strpbrk(start + 1, op)) == NULL){
						//row뒤에 start를 붙여넣는다.
						strncat(tokens[row], start, 1);
						start += 1;
					}
					else{
						while(start < end){
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;
							else if(*start != ' ')
								strncat(tokens[row], start, 1);
							start++;	
						}
						if(all_star(tokens[row]))
							row--;
						
					}
				}
			}
			//end가 (일때
			else if(*end == '(') 
			{
				lcount = 0;
				rcount = 0;

				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){
					while(*(end + lcount + 1) == '(')
						lcount++;
					start += lcount;

					end = strpbrk(start + 1, ")");

					if(end == NULL)
						return false;
					else{
						while(*(end + rcount +1) == ')')
							rcount++;
						end += rcount;

						if(lcount != rcount)
							return false;

						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);
							row--;
							start = end + 1;
						}
						else{
							strncat(tokens[row], start, 1);
							start += 1;
						}
					}
						
				}
				else{
					strncat(tokens[row], start, 1);
					start += 1;
				}

			}
			//end가 \일때
			else if(*end == '\"') 
			{
				end = strpbrk(start + 1, "\"");
				
				if(end == NULL)
					return false;

				else{
					strncat(tokens[row], start, end - start + 1);
					start = end + 1;
				}

			}

			//그 외의 경우
			else{
				// ex) a++ ++ +b
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;

				// ex) a-- -- -b
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;
	
				strncat(tokens[row], start, 1);
				start += 1;
				
				// ex) -a or a, -b
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){


					// ex) -a or -a+b
					if(row == 0)
						row--;

					// ex) a+b = -c
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)
							row--;
					}
				}
			}
		}
		else{ 
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--;				

			if(all_star(tokens[row - 1]) && row == 1)   
				row--;	

			for(i = 0; i < end - start; i++){
				if(i > 0 && *(start + i) == '.'){
					strncat(tokens[row], start + i, 1);

					while( *(start + i +1) == ' ' && i< end - start )
						i++; 
				}
				else if(start[i] == ' '){
					while(start[i] == ' ')
						i++;
					break;
				}
				else
					strncat(tokens[row], start + i, 1);
			}

			if(start[0] == ' '){
				start += i;
				continue;
			}
			start += i;
		}
		//row의 오른쪽 왼쪽이 공백일 경우에	
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));
		//row가 0보다 크고 row의 마지막 문자가 문자이고, row-1의 타입이 2이거나 row-1의 마지막 문자가 1이거나.일떄
		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){
			//row가 1보다 크고, row-2가 (를 포함할때
			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				//row-1이 struct가 아니고 unsigned이 아니면 false를 리턴한다.
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;
			}
			//row가 1이고 row의 마지막 문자가 문자일때
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				//0번째가 extern아니고 unsigned가 아니고 타입이 2가 아닐떄 false를 리턴한다.
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}
			//row가 1보다 크고 row-1의 타입이 2일때
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				//row-2가 unsigned이 아니고 extern이 아닐떄 false를 리턴
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
			
		}
		//row가 0이고 rowrk gcc일때
		if((row == 0 && !strcmp(tokens[row], "gcc")) ){
			//토큰 초기화
			clear_tokens(tokens);
			//토큰 첫번쨰에 str저장
			strcpy(tokens[0], str);	
			return 1;
		} 

		row++;
	}

	//row-1의 전체가 *이고 row가 1보다 크고 row-2의 마지막 문자가 문자일때 row--
	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))  
		row--;				
	//row-1의 전체가 *이고 row가 1일때 row--
	if(all_star(tokens[row - 1]) && row == 1)   
		row--;	

	for(i = 0; i < strlen(start); i++)   
	{
		if(start[i] == ' ')  
		{
			while(start[i] == ' ')
				i++;
			if(start[0]==' ') {
				start += i;
				i = 0;
			}
			else
				row++;
			
			i--;
		} 
		else
		{
			strncat(tokens[row], start + i, 1);
			if( start[i] == '.' && i<strlen(start)){
				while(start[i + 1] == ' ' && i < strlen(start))
					i++;

			}
		}
		//row에 왼쪽 오른쪽 공백을 없애고 저장
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));
		//row가 lpthread이고, row가 0보다 크고 row-1이 -일떄
		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){ 
			strcat(tokens[row - 1], tokens[row]);
			memset(tokens[row], 0, sizeof(tokens[row]));
			row--;
		}
		//row가 0보다 크고 row의 마지막 문자가 문자이고 row-1의 타입이 2이거나 마지막 문자가 문자이거나 .일떄
	 	else if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.') ){
			//row가 1보다 크거나 row-2가 (이 아닐떄
			if(row > 1 && strcmp(tokens[row-2],"(") == 0)
			{
				//row-1이 struct가 아니고 unsigned가 아닐때 false를 리턴한다.
				if(strcmp(tokens[row-1], "struct") != 0 && strcmp(tokens[row-1], "unsigned") != 0)
					return false;
			}
			//row가 1이고 row의 마지막 문자가 문자일때
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				//0번째가 extern이 아니거나 unsigned가 아니거나 타입이 2가 아닐떄 false를 리턴한다.
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
		} 
	}


	if(row > 0)
	{

		// ex) #include <sys/types.h>
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){ 
			clear_tokens(tokens); 
			strcpy(tokens[0], remove_extraspace(str)); 
		}
	}

	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){
		for(i = 1; i < TOKEN_CNT; i++){
			if(strcmp(tokens[i],"") == 0)  
				break;		       

			if(i != TOKEN_CNT -1 )
				strcat(tokens[0], " ");
			strcat(tokens[0], tokens[i]);
			memset(tokens[i], 0, sizeof(tokens[i]));
		}
	}
	
	//change ( ' char ' )' a  ->  (char)a
	while((p_str = find_typeSpecifier(tokens)) != -1){ 
		if(!reset_tokens(p_str, tokens))
			return false;
	}

	//change sizeof ' ( ' record ' ) '-> sizeof(record)
	while((p_str = find_typeSpecifier2(tokens)) != -1){  
		if(!reset_tokens(p_str, tokens))
			return false;
	}
	
	return true;
}

//트리를 만들어 주는 함수
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)
{
	//현재 노드에 입력받은 노드를 저장
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)	
	{
		//idx위치가 비어있으면 반복문을 종료해준다.
		if(strcmp(tokens[*idx], "") == 0)
			break;
		//idx위치에 )일때 현재 노드의 최상위 노드를 리턴하고 종료한다.
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur);
		//idx위치에 ,일때 최상위 root노드를 리턴하고 종료한다.
		else if(!strcmp(tokens[*idx], ","))
			return get_root(cur);
		//idx위치에 (일때
		else if(!strcmp(tokens[*idx], "("))
		{
			//idx가 0이상이고 idx-1위치의 연산자이고 ,아닐때 
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){
				fstart = true;

				while(1)
				{
					*idx += 1;
					//idx가 )이면 종료
					if(!strcmp(tokens[*idx], ")"))
						break;
					
					//재귀호출한 값을 넣어준다.
					new = make_tree(NULL, tokens, idx, parentheses + 1);
					
					if(new != NULL){
						//fstart가 true일때 현재 노드가 새로운 노드의 상위가 된다.
						if(fstart == true){
							cur->child_head = new;
							new->parent = cur;
	
							fstart = false;
						}
						//아니면 새로운 노드의 하위가 된다.
						else{
							cur->next = new;
							new->prev = cur;
						}

						cur = new;
					}

					if(!strcmp(tokens[*idx], ")"))
						break;
				}
			}
			else{
				*idx += 1;

				new = make_tree(NULL, tokens, idx, parentheses + 1);

				if(cur == NULL)
					cur = new;
				//새로운 노드의 name과 현재노드의 name이 같으면
				else if(!strcmp(new->name, cur->name)){
					//새로운 노드의 name이 |, ||, &, &&중 하나 일 경우
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{
						//현재노드의 마지막 자식을 찾는다.
						cur = get_last_child(cur);
						//새로운 노드의 자식노드가 존재한다면 자식노드와는 이웃하는 관계가 된다.
						if(new->child_head != NULL){
							new = new->child_head;

							new->parent->child_head = NULL;
							new->parent = NULL;
							new->prev = cur;
							cur->next = new;
						}
					}
					//새로운 노드의 name이 +, *중 하나면
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{
						i = 0;

						while(1)
						{
							//idx+i위치가 비어있다면 반복문을 종료한다.
							if(!strcmp(tokens[*idx + i], ""))
								break;
							//idx+i위치가 연산자이고 )라면 반복문을 종료한다.
							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)
								break;

							i++;
						}
						//idx+1위치와 new노드의 name의 연산자 우선순위를 비교한다.
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							//현재 노드를 가장 막내가 현재 노드가 된다.
							cur = get_last_child(cur);
							cur->next = new;//다음위치에 새로운 노드가 들어가고
							new->prev = cur;//새로운노드 이전 위치에 현재노드가 저장된다.
							cur = new;//새로운노드를 현재노드로 한다.
						}
						else
						{
							//현재 노드를 막내 노드로 되낟.
							cur = get_last_child(cur);

							if(new->child_head != NULL){
								new = new->child_head;

								new->parent->child_head = NULL;
								new->parent = NULL;
								new->prev = cur;
								cur->next = new;
							}
						}
					}
					else{
						cur = get_last_child(cur);
						cur->next = new;
						new->prev = cur;
						cur = new;
					}
				}
	
				else
				{
					cur = get_last_child(cur);

					cur->next = new;
					new->prev = cur;
	
					cur = new;
				}
			}
		}
		//idx의 위치에 연산자 이면
		else if(is_operator(tokens[*idx]))
		{
			//idx가 ||, &&, |, &, +, *중 하나 이면
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{
				//cur->name이 연산자이고 idx면 operator에 cur노드를 저장한다.
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur;
		
				else
				{
					//idx토큰을 이용해서 새 노드를 생성한다.
					new = create_node(tokens[*idx], parentheses);
					//가장 우선순위가 높은 연산자를 operator에 저장한다.
					operator = get_most_high_precedence_node(cur, new);
					//operator의 부모가 없거나 이전이 없으면(최상위일경우)
					if(operator->parent == NULL && operator->prev == NULL){
						//opertor->name의 연산자 우선순위를 비교한다.
						if(get_precedence(operator->name) < get_precedence(new->name)){
							//new의 자식노드가 된다.
							cur = insert_node(operator, new);
						}
						//operator가 우선순위가 앞서면
						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							//마지막 노드가 된다.
							if(operator->child_head != NULL){
								operator = get_last_child(operator);
								cur = insert_node(operator, new);
							}
						}
						//같을경우
						else
						{
							//현재노드를 operator에 넣고
							operator = cur;
	
							while(1)
							{
								//operator->name이 연산자 이거나 idx면 반복문 종료
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))
									break;
						
								if(operator->prev != NULL)
									operator = operator->prev;
								else
									break;
							}
							//idx랑 같으면
							if(strcmp(operator->name, tokens[*idx]) != 0)
								operator = operator->parent;

							if(operator != NULL){
								if(!strcmp(operator->name, tokens[*idx]))
									cur = operator;
							}
						}
					}

					else
						cur = insert_node(operator, new);
				}

			}
			//연산자가 아닐때
			else
			{
				//토큰으로 새 노드를 생성한다.
				new = create_node(tokens[*idx], parentheses);

				if(cur == NULL)
					cur = new;

				else
				{
					//operator를 new보다 연산자 우선순위가 가장 높은 노드로 이동
					operator = get_most_high_precedence_node(cur, new);
					//operator->parenthese가 new ->parentheses보다 높을경우 노드를 삽입
					if(operator->parentheses > new->parentheses)
						cur = insert_node(operator, new);
					//부모노드가 없고 이전이 없는 최상위 노드일때
					else if(operator->parent == NULL && operator->prev ==  NULL){
						//op연산자가 new연산자보다 우선순위가 높을때
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){
	
								operator = get_last_child(operator);
								cur = insert_node(operator, new);
							}
						}
						//new가 우선순위가 높으면 new에 삽입
						else
							cur = insert_node(operator, new);
					}
	
					else
						cur = insert_node(operator, new);
				}
			}
		}
		else 
		{
			//idx를 가지고 새로운 노드 생성
			new = create_node(tokens[*idx], parentheses);

			if(cur == NULL)
				cur = new;

			else if(cur->child_head == NULL){
				cur->child_head = new;
				new->parent = cur;

				cur = new;
			}
			else{

				cur = get_last_child(cur);

				cur->next = new;
				new->prev = cur;

				cur = new;
			}
		}

		*idx += 1;
	}
	//root노드를 리턴
	return get_root(cur);
}
//좌우 노드를 변환하는 함수입니다.
node *change_sibling(node *parent)
{
	node *tmp;
	//tmp자식노드를 설정한다.
	tmp = parent->child_head;
	//자식노드를 자식의 next로 설정한다.
	parent->child_head = parent->child_head->next;
	//자식노드의 부모를 parent로 설정해준다.
	parent->child_head->parent = parent;
	//자식노드의 이전를 초기화한다.
	parent->child_head->prev = NULL;
	//자식노드의 다음을 tmp로 설정한다.
	parent->child_head->next = tmp;
	//자식노드의 이전을 자식노드로 설정
	parent->child_head->next->prev = parent->child_head;
	//자식노드의 next의 다음을 초기화
	parent->child_head->next->next = NULL;
	//자식노드의 next의 부모를 초기화
	parent->child_head->next->parent = NULL;		

	return parent;
}
//노드를 생성하는 함수입니다.
node *create_node(char *name, int parentheses)
{
	node *new;
	//new노드를 동적할당
	new = (node *)malloc(sizeof(node));
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(new->name, name);

	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	return new;
}
//연산자 우선순위를 리턴하는 함수입니다.
int get_precedence(char *op)
{
	int i;

	for(i = 2; i < OPERATOR_CNT; i++){
		if(!strcmp(operators[i].operator, op))
			return operators[i].precedence;
	}
	return false;
}
//문자열을 입력받으면 연산자인지 확인시켜주는 함수입니다.
int is_operator(char *op)
{
	int i;

	for(i = 0; i < OPERATOR_CNT; i++)
	{
		if(operators[i].operator == NULL)
			break;
		if(!strcmp(operators[i].operator, op)){
			return true;
		}
	}

	return false;
}
//	노드의 이름을 출력해주는 함수입니다.
void print(node *cur)
{
	//자식노드 출력
	if(cur->child_head != NULL){
		print(cur->child_head);
		printf("\n");
	}
	//다음노드출력
	if(cur->next != NULL){
		print(cur->next);
		printf("\t");
	}
	printf("%s", cur->name);
}
//연산자가 있는 노드를 얻기위한 함수입니다.
node *get_operator(node *cur)
{
	if(cur == NULL)
		return cur;

	if(cur->prev != NULL)
		while(cur->prev != NULL)
			cur = cur->prev;

	return cur->parent;
}
//최상의 노드를 반환시키기 위한 함수입니다.
node *get_root(node *cur)
{
	if(cur == NULL)
		return cur;

	while(cur->prev != NULL)
		cur = cur->prev;

	if(cur->parent != NULL)
		cur = get_root(cur->parent);

	return cur;
}
//연산자 우선순위가 높은 노드를 얻기 위한 함수입니다.
node *get_high_precedence_node(node *cur, node *new)
{
	if(is_operator(cur->name))
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur;

	if(cur->prev != NULL){
		while(cur->prev != NULL){
			cur = cur->prev;
			
			return get_high_precedence_node(cur, new);
		}


		if(cur->parent != NULL)
			return get_high_precedence_node(cur->parent, new);
	}

	if(cur->parent == NULL)
		return cur;
}
//연산자 우선순위가 가장 높은 노드를 얻기 위한 함수입니다.
node *get_most_high_precedence_node(node *cur, node *new)
{
	node *operator = get_high_precedence_node(cur, new);
	node *saved_operator = operator;

	while(1)
	{
		if(saved_operator->parent == NULL)
			break;

		if(saved_operator->prev != NULL)
			operator = get_high_precedence_node(saved_operator->prev, new);

		else if(saved_operator->parent != NULL)
			operator = get_high_precedence_node(saved_operator->parent, new);

		saved_operator = operator;
	}
	
	return saved_operator;
}
//해당 old노드에 new를 삽입하기 위한 함수입니다.
node *insert_node(node *old, node *new)
{
	if(old->prev != NULL){
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	new->child_head = old;
	old->parent = new;

	return new;
}
//가장 하위의 노드를 반환하기 위한 함수입니다.
node *get_last_child(node *cur)
{
	if(cur->child_head != NULL)
		cur = cur->child_head;

	while(cur->next != NULL)
		cur = cur->next;

	return cur;
}
//이웃하는 노드의 수를 리턴해주는 함수힙니다.
int get_sibling_cnt(node *cur)
{
	int i = 0;

	while(cur->prev != NULL)
		cur = cur->prev;

	while(cur->next != NULL){
		cur = cur->next;
		i++;
	}

	return i;
}
//노드를 초기화 해주는 함수 입니다.
void free_node(node *cur)
{
	if(cur->child_head != NULL)
		free_node(cur->child_head);

	if(cur->next != NULL)
		free_node(cur->next);

	if(cur != NULL){
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);
	}
}

//문자인지 확인하는 함수입니다.
int is_character(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
//입력받은 문자열의 타입을 구분해주는 함수입니다.
int is_typeStatement(char *str)
{ 
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	//start에 str 포인터를 저장한다.
	start = str;
	//st2에 str길이만큼 str을 복사해서 저장한다.
	strncpy(str2,str,strlen(str));
	//str2의 공백을 제거한다.
	remove_space(str2);

	//start의 첫번째가 공백이면 공백이 나오지 않을때까지 한칸씩 이동시켜준다.
	while(start[0] == ' ')
		start += 1;
	//str2가 gccc를 포함하고 있지 않으면
	if(strstr(str2, "gcc") != NULL)
	{
		//gcc의 길이만큼 tmp2에 start를 복사하여 저장한다.
		strncpy(tmp2, start, strlen("gcc"));
		//tmp2가 gcc가 아니면 0을 리턴 맞으면 2를 리턴
		if(strcmp(tmp2,"gcc") != 0)
			return 0;
		else
			return 2;
	}
	
	for(i = 0; i < DATATYPE_SIZE; i++)
	{
		//str2가 dataype[i]를 포함한다면
		if(strstr(str2,datatype[i]) != NULL)
		{	
			//tmp에 datatype[i]길이 만큼 str2를 복사하여 저장한다.
			strncpy(tmp, str2, strlen(datatype[i]));
			//tmp2에 datatype[i]길이 만큼 start를 복사하여 저장한다.
			strncpy(tmp2, start, strlen(datatype[i]));
			
			//tmp가 datatype[i]가 아닌 경우
			if(strcmp(tmp, datatype[i]) == 0)
				//tmp와 tmp2가 다른 경우 0을 리턴 같으면 2를 리턴한다.
				if(strcmp(tmp, tmp2) != 0)
					return 0;  
				else
					return 2;
		}

	}
	return 1;

}
//타입을 구분 시켜주는 함수입니다.
int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) 
{
	int i, j;

	for(i = 0; i < TOKEN_CNT; i++)
	{
		for(j = 0; j < DATATYPE_SIZE; j++)
		{
			//데이터타입과 일치하는 토큰이 있고 i가 0보다 클때
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0)
			{
				//i-1이 (이고 i+1이 )이고 i+2가 &, *, ), (, -, +, 문자중 하나일때 return i를 한다.
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))  
					return i;
			}
		}
	}
	return -1;
}
//타입을 구분 시켜주는 함수입니다.
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) 
{
    int i, j;

   
    for(i = 0; i < TOKEN_CNT; i++)
    {
        for(j = 0; j < DATATYPE_SIZE; j++)
        {
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1]))  
                    return i;
        }
    }
    return -1;
}
//해당 문자열이 전체가 *인지 확인하는 함수입니다.
int all_star(char *str)
{
	int i;
	int length= strlen(str);
	
 	if(length == 0)	
		return 0;
	
	for(i = 0; i < length; i++)
		if(str[i] != '*')
			return 0;
	return 1;

}
//해당 문자열이 전체가 문자인지 확인하는 함수입니다.
int all_character(char *str)
{
	int i;

	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}
//토큰을 리셋 해주는 함수입니다.
int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) 
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if(start > -1){
		//토큰에 struct가 있을때
		if(!strcmp(tokens[start], "struct")) {
			//공백을 삽입하고 start+1번째를 start뒤에 삽입한다.		
			strcat(tokens[start], " ");
			strcat(tokens[start], tokens[start+1]);	     

			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				//i번째 토큰에 i+1번째 토큰을 저장한다.
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}
		//토큰이 unsigned이고 start+1위치에 )인 경우
		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {		
			strcat(tokens[start], " ");
			strcat(tokens[start], tokens[start + 1]);	     
			strcat(tokens[start], tokens[start + 2]);

			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

     		j = start + 1;           
        	while(!strcmp(tokens[j], ")")){
                	rcount ++;
                	if(j==TOKEN_CNT)
                        	break;
                	j++;
        	}
	
		j = start - 1;
		while(!strcmp(tokens[j], "(")){
        	        lcount ++;
                	if(j == 0)
                        	break;
               		j--;
		}
		//j가 0이 아니고 token의 j번째가 문자이거나j==0이면 lcount=rcount
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			lcount = rcount;

		//위 조건에 부합하지않으면 false를 리턴
		if(lcount != rcount )
			return false;
		//start-lcount가 0이고 start-lcount-1번째가 sizeof이면 true를 리턴한다.
		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true; 
		}
		//start가 unsigned이거나 struct이고 start+1이 )아닌 경우
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {		
			//start-lcount뒤에 start를 삽입한다.
			strcat(tokens[start - lcount], tokens[start]);
			//start-lcount뒤에 start+1를 삽입한다.
			strcat(tokens[start - lcount], tokens[start + 1]);
			//start-lcount+1에 start+rcount를 저장한다.
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);
	
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount]);
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}


		}
 		else{
			//start+2가 (인경우
			if(tokens[start + 2][0] == '('){
				j = start + 2;
				while(!strcmp(tokens[j], "(")){
					sub_lcount++;
					j++;
				}
				//j+1이 )일때 	
				if(!strcmp(tokens[j + 1],")")){
					j = j + 1;
					while(!strcmp(tokens[j], ")")){
						sub_rcount++;
						j++;
					}
				}
				//그렇지 않으면 false를 return
				else 
					return false;
				//sub_lcount와 sub_rcount가 같지않으면 false를 리턴
				if(sub_lcount != sub_rcount)
					return false;
				
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);	
				for(int i = start + 3; i<TOKEN_CNT; i++)
					memset(tokens[i], 0, sizeof(tokens[0]));

			}
			//start-lcount뒤에 start를 삽입한다.
			strcat(tokens[start - lcount], tokens[start]);
			//start-lcount뒤에 start+1를 삽입한다.
			strcat(tokens[start - lcount], tokens[start + 1]);
			//start-lcount뒤에 start+rcount+1를 삽입한다.
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);
		 
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true;
}
//토큰을 초기화하는 함수입니다.
void clear_tokens(char tokens[TOKEN_CNT][MINLEN])
{
	int i;

	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}
//오른쪽 공백을 지우는 함수입니다.
char *rtrim(char *_str)
{
	char tmp[BUFLEN];
	char *end;

	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1;
	while(end != _str && isspace(*end))
		--end;

	*(end + 1) = '\0';
	_str = tmp;
	return _str;
}
//왼쪽 공백을 지우는 함수입니다.
char *ltrim(char *_str)
{
	char *start = _str;

	while(*start != '\0' && isspace(*start))
		++start;
	_str = start;
	return _str;
}
//공백을 없애기 위한 함수입니다.
char* remove_extraspace(char *str)
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN);
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;
	//str에서 include<가 포함되어 있는지 확인
	if(strstr(str,"include<")!=NULL){
		start = str;
		end = strpbrk(str, "<");
		position = end - start;
	
		strncat(temp, str, position);
		strncat(temp, " ", 1);
		strncat(temp, str + position, strlen(str) - position + 1);

		str = temp;		
	}
	
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] ==' ')
		{
			if(i == 0 && str[0] ==' ')
				while(str[i + 1] == ' ')
					i++;	
			else{
				if(i > 0 && str[i - 1] != ' ')
					str2[strlen(str2)] = str[i];
				while(str[i + 1] == ' ')
					i++;
			} 
		}
		else
			str2[strlen(str2)] = str[i];
	}

	return str2;
}


//공백을 제거하기 위한 함수입니다.
void remove_space(char *str)
{
	char* i = str;
	char* j = str;
	
	while(*j != 0)
	{
		*i = *j++;
		if(*i != ' ')
			i++;
	}
	*i = 0;
}
//괄호가 온전히 있는지 체크하기 위한 함수이비낟.
int check_brackets(char *str)
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){
		//start에 괄호가 있는지 확인
		if((start = strpbrk(start, "()")) != NULL){
			//(일 경우 lcount++해준다 아니면 rcount++한다.
			if(*(start) == '(')
				lcount++;
			else
				rcount++;

			start += 1; 		
		}
		//없으면 break;
		else
			break;
	}
	//온전히 괄호가 있으면 return 1 아니면 0
	if(lcount != rcount)
		return 0;
	else 
		return 1;
}
//토큰의 갯수를 반환하는 함수입니다.
int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
