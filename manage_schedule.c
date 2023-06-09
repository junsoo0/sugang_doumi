#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include "sugang_doumi.h"

enum schedule_choice {
	ENTER_SCHEDULE = 1, DELETE_SCHEDULE, SCHEDULE_BACK, SCHEDULE_EXIT
};

enum date_diff {
	PAST, PRESENT, FUTURE
};

void show_schedule(int opt);
void enter_schedule();
void delete_schedule();
void wrong_date();

int manage_schedule() {
	char input[INPUT_SIZE];

	chdir(schedule_path);
	while (1) {
		clear_terminal();
		printf("\x1b[32m");
		printf("[일정 관리]\n");
		printf("\x1b[0m");
		show_schedule(0);
		printf("========================================\n");
		printf("[1] 일정 등록\n");
		printf("[2] 일정 삭제\n");
		printf("[3] 돌아가기\n");
		printf("----------------------------------------\n");
		printf("선택: [1 - 3] ");

		fgets(input, INPUT_SIZE, stdin);
		input[strlen(input) - 1] = '\0';
		if (check_valid_input(input, 3) == FALSE) {
			sleep(1);
			continue;
		}
		puts("");

		switch (input[0] - '0') {
			case ENTER_SCHEDULE:
				enter_schedule();
				break;
			case DELETE_SCHEDULE:
				delete_schedule();
				break;
		}

		if (input[0] - '0' == SCHEDULE_BACK)
			break;
	}
	
	chdir("..");

	return 0;
}

void show_schedule(int opt) {
	int fcnt;
	int i, j;
	int cy, cm, cd, y, m, d;
	int date_flag;
	int flag = FALSE;
	char str[INPUT_SIZE], strf[INPUT_SIZE];
	FILE  *fp;
	struct dirent **flist;
	time_t t;
	struct tm *tp;

	if (opt > 0) { // main에서 접속한 경우
		if (chdir(schedule_path) == -1) {
			mkdir("schedule", FOLDER_PERMISSION);
			chdir(schedule_path);
		}
	}

	t = time(NULL);
	tp = localtime(&t);
	cy = tp->tm_year + 1900;
	cm = tp->tm_mon + 1;
	cd = tp->tm_mday;

	// 파일 목록 이름순으로 정렬해서 배열에 저장
	fcnt = scandir(".", &flist, NULL, alphasort);

	i = 0;
	for (j = 0; j < fcnt; j++) {
		if (strcmp(flist[j]->d_name, ".") == 0 || strcmp(flist[j]->d_name, "..") == 0)
			continue;

		if (flag == FALSE) {
			if (opt > 0)
				printf("마감일자순 %d개의 일정을 표시합니다.\n", opt);
			else
				printf("전체 일정을 표시합니다.\n");

			printf("========================================\n");
			flag = TRUE;
		}
		else
			puts("");

		fp = fopen(flist[j]->d_name, "r");
		fgets(str, INPUT_SIZE, fp);
		printf("(%d) %s", ++i, str);
		fgets(str, INPUT_SIZE, fp);
		strcpy(strf, str);

		strf[2] = strf[5] = '\0';
		y = atoi(&strf[0]) + 2000;
		m = atoi(&strf[3]);
		d = atoi(&strf[6]);

		date_flag = FUTURE;
		if (y < cy)
			date_flag = PAST;
		else if (y == cy) {
			if (m < cm)
				date_flag = PAST;
			else if (m == cm) {
				if (d < cd)
					date_flag = PAST;
				else if (d == cd)
					date_flag = PRESENT;
			}
		}
		str[8] = '\0';
		printf("날짜: %s", str);
		if (date_flag == PAST) {
			printf("\x1b[33m");
			printf(" 마감\n");
			printf("\x1b[0m");
		}
		else if (date_flag == PRESENT) {
			printf("\x1b[31m");
			printf(" D-Day\n");
			printf("\x1b[0m");
		}
		else
			puts("");

		fgets(str, INPUT_SIZE, fp);
		printf("내용: %s", str);
		fclose(fp);

		if (opt > 0 && opt == i)
			break;
	}
	
	if (i == 0) {
		printf("========================================\n");
		printf("등록된 일정이 없습니다.\n");
	}

	for (j = 0; j < fcnt; j++)
		free(flist[j]);
	free(flist);

	if (opt > 0) // main에서 접속한 경우
		chdir("..");
}

void enter_schedule() {
	int year, month, day;
	char i = '1';
	char fname[INPUT_SIZE], title[INPUT_SIZE], due_date[INPUT_SIZE], parse[INPUT_SIZE], contents[INPUT_SIZE];
	FILE *fp;
	DIR *dp;
	struct dirent *direntp;


	printf("일정 이름: ");
	fgets(title, INPUT_SIZE, stdin);

	while (1) {
		printf("마감 날짜: [yy-mm-dd] 8자리 ex) 23-06-01 ");
		fgets(due_date, INPUT_SIZE, stdin);
		strcpy(fname, due_date);
		fname[strlen(fname) - 1] = '\0';
		strcat(fname, "_1");
		strcpy(parse, due_date);
		if (parse[2] != '-' || parse[5] != '-') {
			wrong_date();
			continue;
		}

		parse[2] = '\0';
		parse[5] = '\0';
		year = atoi(&parse[0]);
		month = atoi(&parse[3]);
		day = atoi(&parse[6]);

		if (year <= 0 || year >= 100 || month <= 0 || month >= 13 || day <= 0 || day >= 32) {
			wrong_date();
			continue;
		}
		else if ((month == 4 || month == 6 || month == 9 || month == 11) && day >= 31) {
			wrong_date();
			continue;
		}
		else if (month == 2 && day >= 30) {
			wrong_date();
			continue;
		}

		break;
	}

	printf("내용: \n");
	fgets(contents, INPUT_SIZE, stdin);

	dp = opendir(".");
	while ((direntp = readdir(dp)) != NULL) {
		if (strcmp(direntp->d_name, fname) == 0) {
			fname[strlen(fname) - 1] = ++i;
		}
	}
	closedir(dp);

	fp = fopen(fname, "w");
	fwrite(title, strlen(title), sizeof(char), fp);
	fwrite(due_date, strlen(due_date), sizeof(char), fp);
	fwrite(contents, strlen(contents), sizeof(char), fp);
	fclose(fp);

	printf("일정이 등록되었습니다.\n");
	sleep(1);
}

void delete_schedule() {
	int fcnt;
	int j;
	char input[INPUT_SIZE];
	struct dirent **flist;

	fcnt = scandir(".", &flist, NULL, alphasort);
	while (1) {
		if (fcnt - 2 == 0) {
			printf("등록된 일정이 없습니다.\n");
			sleep(1);
			break;
		}
		printf("삭제할 일정 번호 입력: [1 - %d] ", fcnt - 2);
		fgets(input, INPUT_SIZE, stdin);
		input[strlen(input) - 1] = '\0';
		if (check_valid_input(input, fcnt - 2) == FALSE)
			continue;

		if (remove(flist[atoi(input) + 1]->d_name) == 0)
			printf("일정이 삭제되었습니다.\n");
		else
			printf("일정 삭제 오류!\n");
		sleep(1);

		break;
	}

	for (j = 0; j < fcnt; j++)
		free(flist[j]);
	free(flist);

}

void wrong_date() {
	puts("");
	printf("잘못된 날짜형식입니다.\n");
	printf("다시 입력해주세요.\n");
	puts("");
}
