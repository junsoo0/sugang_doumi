#define _CRT_SECURE_NO_WARNINGS  

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "sugang_doumi.h"

int my_post();
int show_post();
int write_post();

const char* folderPath = "../post_lst";

// char user_path[INPUT_SIZE] = "./2021112563";

int free_board() {
	int input_num;
	
	while (1) {
		printf("\x1b[32m");
		clear_terminal();
		printf("[자유게시판]\n");
		printf("\x1b[0m");
		printf("========================================\n");
		printf("[1] 게시글 보기\n");
		printf("[2] 게시글 작성\n");
		printf("[3] 내가 쓴 글 보기, 삭제하기\n");
		printf("[4] 돌아가기\n");
		printf("----------------------------------------\n");
	
		printf("선택: [1 - 4] ");
		if ((scanf("%d", &input_num) != 1) || (input_num > 4) || (input_num < 1)) {
			getchar();
			printf("잘못된 입력입니다. 1, 2, 3, 4 중 하나를 입력하세요.\n");
			sleep(1);
			continue;
		}

		getchar();
		puts("");
	
		switch(input_num) {
			case 1:
				show_post();
				sleep(1);
				continue;
			case 2:
				write_post();
				sleep(1);
				continue;
			case 3:
				my_post();
				sleep(1);
				continue;
			case 4:
				return 0;
			default:
				break;
		}
	
	}
	return 0;	
}


int write_post()
{
	// 사용자가 작성을 선택
	char new_title[INPUT_SIZE];
	FILE *fp;
	char new_text[INPUT_SIZE];
	
	struct stat st;
	if (stat(folderPath, &st) != 0 || !S_ISDIR(st.st_mode)) {
		if (mkdir(folderPath, 0700) != 0) {
			perror("make folder fail");
			return 1;
		}
	}
	
	DIR* dir = opendir(folderPath);
	if (dir == NULL) {
		perror("open folder fail");
		return 1;
	}
	
	struct dirent* entry;
	char temp_name[INPUT_SIZE];
	int temp_num, max_num = 1, temp_len, new_len;
	while ((entry = readdir(dir)) != NULL) {
		strcpy(temp_name, entry->d_name);
		temp_len = strlen(temp_name);
		new_len = temp_len - 4;
		temp_name[new_len] = '\0';
		temp_num = atoi(temp_name);
		
		if (temp_num >= max_num) max_num = temp_num + 1;
	}
	
	// 작성할 post 제목과 내용 입력받음
	printf("title : ");
	fgets(new_title, INPUT_SIZE, stdin);
	
	
	char filename[20];    // file open
	sprintf(filename, "%s/%d.txt", folderPath, max_num);
	
	fp = fopen(filename, "w");
	if (fp < 0) {
		perror("File open fail");
		exit(1);
	}

	fprintf(fp, "%s", new_title);
	fprintf(fp, "%s\n\n", user_path);
	printf("text (종료는 q): \n");	// file writing
	while (1) {
		fgets(new_text, sizeof(new_text), stdin);
		if (!strcmp(new_text, "q\n")) break;
		
		size_t len = strlen(new_text);    // 입력 끝의 개행 제거
        	if (len > 0 && new_text[len - 1] == '\n')
            	new_text[len - 1] = '\0';
            	
		fprintf(fp, "%s\n", new_text);
		// write(fp, new_text, sizeof(new_text));
	}
	fclose(fp);
	
	printf("\n작성이 완료되었습니다.\n");
		
	return 0;
}
	
	
int show_post() {
	int input_num, cnt = 0;
	char filePath[INPUT_SIZE];
	char line[INPUT_SIZE];
	
	struct dirent **namelist;
	int count = 0;
	
	if ((count = scandir(folderPath, &namelist, NULL, alphasort) ) == -1) {
		perror("folder open error");
		return 0;
	}
	
	for (int i = (count - 1); i >= 0; i--) {
		
		if (namelist[i]->d_type == DT_REG) {
			strcpy(filePath, folderPath);
			strcat(filePath, "/");
			strcat(filePath, namelist[i]->d_name);
			// 혹시 에러나면 d_name에 끝에 줄바꿈문자 제거하고 다시 시도
			
			FILE* fp = fopen(filePath, "r");
			if (fp != NULL) {
				cnt++;
				printf("%.*s", (int)(strlen(namelist[i]->d_name) - 4), namelist[i]->d_name);
				
				if (fgets(line, INPUT_SIZE, fp) != NULL) {
					printf(". %s\n", line);   // 제목 출력
				}
				
				fclose(fp);
			}
		}
	}
	
	if (cnt == 0)
		printf("\n게시글이 존재하지 않습니다.\n\n");
	
	for(int i = 0; i < count; i++) {    // 데이터 메모리 해제
        free(namelist[i]);
    	}
    	
    	free(namelist);
	
print_post:
	printf("읽을 포스트의 번호 입력(이전으로 돌아가기는 0번) : ");
	if (scanf("%d", &input_num) != 1) {
		getchar();
		printf("잘못된 입력입니다. 숫자를 입력해주세요.\n\n");
		goto print_post;
	}
	
	getchar();
	if (input_num == 0) return 0;
	
	snprintf(filePath, INPUT_SIZE, "%s/%d.txt", folderPath, input_num);
	
	
	FILE* fp = fopen(filePath, "r");
	if (fp == NULL) {
		printf("존재하지 않는 글입니다.\n\n");
		goto print_post;
	}
	else {
		printf("----------------------------------------------\n");
		printf("Title : ");
		fgets(line, INPUT_SIZE, fp);
		printf("%s", line);
		fgets(line, INPUT_SIZE, fp);    // 학번 줄 미출력
		
		while (fgets(line, INPUT_SIZE, fp) != NULL) printf("%s", line);
		
		fclose(fp);
		printf("----------------------------------------------\n\n");
		
		goto print_post;
	}
	
	return 0;
}




int my_post() 
{
	const char* folderPath = "../post_lst";
	char filePath[INPUT_SIZE];
	char temp_userPath[INPUT_SIZE];
	char line[INPUT_SIZE];
	char quit;

	strcpy(temp_userPath, user_path);
	strcat(temp_userPath, "\n");
	printf("나의 작성 목록 : \n\n");
	
	struct dirent **namelist;
	int count = 0, cnt = 0;
	printf("----------------------------------------\n");
	
	if ((count = scandir(folderPath, &namelist, NULL, alphasort) ) == -1) {
		perror("folder open error");
		return 0;
	}
	
	for (int i = (count - 1); i >= 0; i--) {
		//printf("%s\n", namelist[i]->d_name);
		
		if (namelist[i]->d_type == DT_REG) {
			strcpy(filePath, folderPath);
			strcat(filePath, "/");
			strcat(filePath, namelist[i]->d_name);
			// 혹시 에러나면 d_name에 끝에 줄바꿈문자 제거하고 다시 시도
			
			FILE* fp = fopen(filePath, "r");    // 파일 열기
			if (fp != NULL) {
				cnt++;
				
				fgets(line, INPUT_SIZE, fp);
				fgets(line, INPUT_SIZE, fp);    // 작성자 정보가 2번째줄에 있으므로 2번째 줄 가져오기
				if (!strcmp(line, temp_userPath)) {    // 작성자 일치
					printf("%.*s. ", (int)(strlen(namelist[i]->d_name) - 4), namelist[i]->d_name);
					fseek(fp, 0, SEEK_SET);    // 파일포인터 처음으로 이동
					printf("Title: ");
					fgets(line, INPUT_SIZE, fp);
					printf("%s", line);
					fgets(line, INPUT_SIZE, fp);    // 작성자 정보 스킵
					
					// 내용 출력
					while (fgets(line, INPUT_SIZE, fp) != NULL) 
						printf("%s", line);
						
					printf("----------------------------------------\n");
					
				}
			}
			fclose(fp);
		}
	}
	
	if (cnt == 0)
		printf("\n작성한 글이 존재하지 않습니다.\n\n");
	
	for(int i = 0; i < count; i++) {    // 데이터 메모리 해제
        	free(namelist[i]);
    	}
    	
    	free(namelist);
	
	

	int del_num, x;	
do_delete:
	printf("삭제할 글 번호 선택(무시하려면 0 입력) : ");
	x = scanf("%d", &del_num);
	getchar();
	
	if (x != 1) {
		printf("잘못된 입력입니다. 다시 입력하세요.\n\n");
		goto do_delete;
	}
	
	if (del_num == 0) goto do_quit;
	
	snprintf(filePath, INPUT_SIZE, "%s/%d.txt", folderPath, del_num);
	
	if (access(filePath, F_OK) == -1) {    // 입력받은 파일이 존재하는지 확인
        	printf("존재하지 않는 파일입니다. 다시 입력하세요.\n\n");
        	goto do_delete;
    	}
    	
    	FILE* fp = fopen(filePath, "r");
    	
    	// 파일에 저장된 학번과 user_path 비교
    	char user[INPUT_SIZE];
    	fgets(user, INPUT_SIZE, fp);
    	fgets(user, INPUT_SIZE, fp);
    	
    	size_t len = strlen(user);    // 입력 끝의 개행 제거
        if (len > 0 && user[len - 1] == '\n')
            user[len - 1] = '\0';
            
        fclose(fp);
        
        // user_path와 비교하여 같을 경우 파일 삭제
    	if (strcmp(user, user_path) == 0) {
        	if (remove(filePath) == 0) {
            		printf("삭제가 완료되었습니다.\n\n");
            		goto do_delete;
        	}
        	else {
            		perror("delete file fail");
            		return 1;
        	}
    	} 
    	else {
        	printf("다른 유저가 작성한 게시글로, 삭제할 수 없습니다.\n다시 입력해주세요.\n");
        	goto do_delete;
    	}
	
	
	
do_quit:
	printf("\nq 입력 시 메뉴로 돌아갑니다 : ");
	scanf("%c", &quit);
	getchar();
	
	if (quit == 'q') return 0;
	else goto do_quit;
	
}
