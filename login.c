#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

int login();
int sign_up();
int passwd_find();

char id[300];
char home[300] = "/home/sudal/sp/tp";
char account_path[300] = "/home/sudal/sp/tp/account";


int main(){

	int condition = 1;
	int login_ans;
	mkdir(home, 0777);
	mkdir(account_path, 0777);
	

	
	
	while(condition){
		puts("");
		printf("1. 로그인   2. 회원가입   3. 비밀번호 찾기   4.종료\n");
		printf("원하는 메뉴를 입력하세요(1~4): ");
		scanf("%d", &login_ans);
		getchar();

		switch(login_ans){
			case 1 : {
			         int a = login();
				 if(a != -1) condition = 0;
				 break;
			      	 	}
			case 2 : {
				 int b = sign_up();
				 if(b != -1) condition = 0;
			         break;	 
					 }
			case 3 : passwd_find();
				 condition = 0;
				 break;
			case 4 : exit(0);
			default : printf("잘못된 입력입니다.\n");
		}
		

	}


	chdir(home);
	printf("end test\n");
	



}


int login(){
	char buf[300];
	char passwd[300];
	char login_id[300];
	char ans;
	struct termios info;
	tcgetattr(0, &info);
	int pid = fork();
	if(pid == 0){
		execlp("clear", "clear", NULL);
	}
	wait(NULL);

	chdir(home);

	printf("ID를 입력하세요(학번): ");
	scanf("%s", login_id);
	getchar();

	char newpath[300];
	strcpy(newpath, account_path);
	strcat(newpath, "/");
	strcat(newpath, login_id);


	if(chdir(newpath) == -1){
		printf("존재하지 않는 ID(학번)입니다.\n");
	        printf("3초 후 메뉴로 돌아갑니다.\n");
       		sleep(3);
		return -1;		
	}
	else{
label:
		puts("");
		printf("비밀번호를 입력하세요: ");
		info.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &info);
		scanf("%s", passwd);
		getchar();


		info.c_lflag |= ECHO;
		tcsetattr(0, TCSANOW, &info);
		
		// 프로그램 재실행시 buf가 초기화가 안됨 그래서 초기화 해줘야함
		for(int i = 0; i < 300; i++) buf[i] = '\0';
		int passwd_fd = open("passwd", O_RDONLY);
		read(passwd_fd, buf, 300);
		close(passwd_fd);

	
	
		if(strcmp(buf, passwd) == 0){
			puts("");
			printf("로그인 성공.\n");
			// 무한루프 깨고 로그인 성공 후 화면으로 이동
			return 0;
		}
		else{
			while(1){
			puts("");
			printf("비밀번호가 일치하지 않습니다.\n");
			printf("메뉴로 돌아가시겠습니까? (Y/N) : ");
			ans = getchar();
			getchar();

			switch(ans){
				case 'Y' : return -1;
				case 'y' : return -1;
			        case 'N' : goto label;
				case 'n' : goto label;
				default: break;	   
			}
			}

		}
	}


	return 0;
}

int sign_up(){
	char sign_up_id[300];
	char newpath[300];
	char passwd[300];
	char name[300];
	char school[300];
	struct termios info;
	tcgetattr(0, &info);
	int pid = fork();
	if(pid == 0){
		execlp("clear", "clear", NULL);
	}
	wait(NULL);

	chdir(home);

	printf("회원가입 하실 ID를 입력하세요(학번): ");
	scanf("%s", sign_up_id);
	getchar();
	strcpy(newpath, account_path);
	strcat(newpath, "/");
	strcat(newpath, sign_up_id);
	if(chdir(newpath) != -1){
		printf("이미 존재하는 ID입니다.\n");
		printf("3초 후 메뉴로 돌아갑니다.\n");
		sleep(3);
		return -1;
	}
	else
		mkdir(newpath, 0777);

	chdir(newpath);
	puts("");
	printf("비밀번호를 설정하세요: ");
	info.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &info);
	scanf("%s", passwd);
	getchar();

	info.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &info);

	int passwd_fd = creat("passwd", 0777);
	write(passwd_fd, passwd, strlen(passwd));

	puts("");
	puts("");
	printf("당신의 이름을 입력하세요: ");
	scanf("%s", name);
	getchar();
	int name_fd = creat("name", 0777);
	write(name_fd, name, strlen(name));

	puts("");
	printf("당신의 출신 초등학교를 입력하세요(비밀번호 찾기 용도): ");
	scanf("%s", school);
	getchar();
	int school_fd = creat("school", 0777);
	write(school_fd, school, strlen(school));
	
	
	puts("");
	printf("회원가입이 완료되었습니다. 3초 후 메뉴로 돌아갑니다.\n");
	sleep(3);

	close(passwd_fd);
	close(name_fd);
	close(school_fd);

	return -1;
}

int passwd_find(){

	;
}
