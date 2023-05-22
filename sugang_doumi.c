#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sugang_doumi.h"

enum initial_choice {
	LOGIN = 1, FIND_PASSWD, SIGN_UP, INITIAL_EXIT
};

enum main_choice {
	CALCULATE_GPA = 1, EVALUATE_LECTURE, MANAGE_SCHEDULE, CHANGE_PASSWD, LOGOUT, MAIN_EXIT
};

enum credit_choice {
	CREDIT_ADD = 1, CREDIT_REMOVE, CREDIT_LIST, CREDIT_EXIT
};

int initial_UI();
int main_UI();
int check_valid_input(char* input, int total_num);
int login();
int sign_up();
int find_passwd();
int change_passwd();
int logout();
int manage_schedule();
int credit_UI();
int credit_add();
int credit_remove();
int credit_list();
int clear_terminal();

int main() {
	int choice;
	int check = -1;

	getcwd(home_path, INPUT_SIZE);

	while (check == -1) {
initial:	
		clear_terminal();
			
		choice = initial_UI();
		switch (choice) {
			case LOGIN:
				check = login();
				break;
			case FIND_PASSWD:
				find_passwd();
				break;
			case SIGN_UP:
				sign_up();
				break;
			case INITIAL_EXIT:
				break;
		}
	}
	
	while (1) {
		clear_terminal();

		choice = main_UI();
		switch (choice) {
			case CALCULATE_GPA:{
				int credit_condition = 1;
				while(credit_condition){
				int credit_choice = credit_UI();
				switch(credit_choice) {
					case CREDIT_ADD:
							credit_add();
							break;
					case CREDIT_REMOVE:
							credit_remove();
							break;
					case CREDIT_LIST:
							credit_list();
							break;
					case CREDIT_EXIT:
							credit_condition = 0;
							break;
				}
				}
				
				break;
				}
			case EVALUATE_LECTURE:
				break;
			case MANAGE_SCHEDULE:
				manage_schedule();
				break;
			case CHANGE_PASSWD:
				change_passwd();
				break;
			case LOGOUT: 
				if(logout()) goto initial;
				break;
			case MAIN_EXIT:
				break;
		}
	}

	return 0;
}

int initial_UI() {
	char input[INPUT_SIZE];

	while (1) {
		printf("========================================\n");
		printf("               수강 도우미              \n");
		printf("========================================\n");
		printf("[1] 로그인\n");
		printf("[2] 비밀번호 찾기\n");
		printf("[3] 회원가입\n");
		printf("[4] 종료\n");
		printf("----------------------------------------\n");
		printf("선택: [1 - 4] ");
		
		fgets(input, INPUT_SIZE, stdin);
		input[strlen(input) - 1] = '\0';
		if (check_valid_input(input, 4) == FALSE)
			continue;
		puts("");
		
		return input[0] - '0';
	}
}

int main_UI() {
	char input[INPUT_SIZE];

	printf("[메인 화면]\n");
	while (1) {
		show_schedule(3);
		printf("========================================\n");
		printf("[1] 학점 계산\n");
		printf("[2] 강의 평가\n");
		printf("[3] 일정 관리\n");
		printf("[4] 비밀번호 변경\n");
		printf("[5] 로그아웃\n");
		printf("[6] 종료\n");
		printf("----------------------------------------\n");
		printf("선택: [1 - 6] ");

		fgets(input, INPUT_SIZE, stdin);
		input[strlen(input) - 1] = '\0';
		if (check_valid_input(input, 6) == FALSE)
			continue;
		puts("");
		
		return input[0] - '0';
	}
}

int credit_UI() {
	clear_terminal();
	
	char input[INPUT_SIZE];

	printf("[학점 계산 메뉴]\n");
	while (1) {
		printf("========================================\n");
		printf("[1] 학점 등록\n");
		printf("[2] 학점 삭제\n");
		printf("[3] 학점 조회\n");
		printf("[4] 돌아가기\n");
		printf("----------------------------------------\n");
		printf("선택: [1 - 4] ");

		fgets(input, INPUT_SIZE, stdin);
		input[strlen(input) - 1] = '\0';
		if (check_valid_input(input, 4) == FALSE)
			continue;
		puts("");
		
		return input[0] - '0';
	}
}

int check_valid_input(char* input, int total_num) {
	if (strlen(input) != 1 || (input[0] - '0') < 1 || (input[0] - '0') > total_num) {
		puts("");
		printf("잘못된 입력입니다.\n");
		printf("다시 입력해주세요.\n");
		puts("");
		return FALSE;
	}
	return TRUE;
}

int clear_terminal() {
	int pid;

	pid = fork();
	if(pid == 0){
		execlp("clear", "clear", NULL);
	}
	wait(NULL);

	return 0;
}
