#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

time_t now;

void printtime(struct utmpx *log){
	char tlog[64];
	struct tm *time;
	time_t t = log->ut_tv.tv_sec;
	time = localtime(&t);
	strftime(tlog, sizeof(tlog), "%a %b %d %H:%M", time);
	printf("%s \n", tlog);
}

void printutmp(struct utmpx *log){
	if(log->ut_type != 1){
		if(strncmp(log->ut_host, "shutdowm", 8) == 0 && strncmp(log->ut_line, "~", 1) == 0){
			log->ut_type = 254;
		}
		switch(log->ut_type){
			case(2):
				strcpy(log->ut_line, "system-boot");
				break;
			case(3):
				strcpy(log->ut_line, "new time");
				break;
			case(4):
				strcpy(log->ut_line, "old time");
				break;
			case(254):
				strcpy(log->ut_line, "shutdown");
				break;
		}
		printf("%-8.8s ", log->ut_user);
		printf("%-12.12s ", log->ut_line);
		printf("%-16.16s ", log->ut_host);
		printtime(log);
	}
}

void printstart(FILE *file){
	struct utmpx buffer;
	fseek(file, 0, SEEK_SET);
	fread(&buffer, sizeof(buffer), 1, file);
	struct tm *time;
	time_t t = buffer.ut_tv.tv_sec;
	time = localtime(&t);
	char start[64];
	strftime(start, sizeof(start), "%a %b %d %H:%M:%S %Y", time);
	printf("\nwtmp begins %s\n", start);
}

int main(int argc, char *argv[]){
	if(argc == 1 ){
		struct utmpx buffer;
		FILE *file = fopen("/var/log/wtmp", "rb");
		fseek(file, 0, SEEK_END);
		int line = ftell(file)/sizeof(buffer) - 1;
		while(line >= 0){
			fseek(file, line*sizeof(buffer), SEEK_SET);
			time(&now);
			fread(&buffer, sizeof(buffer), 1, file);
			printutmp(&buffer);
			line--;
		}
		printstart(file);
		fclose(file);
	}
	else if(argc == 2 && atoi(argv[1]) > 0){
		struct utmpx buffer;
		FILE *file = fopen("/var/log/wtmp", "rb");
		fseek(file, 0, SEEK_END);
		int line = ftell(file)/sizeof(buffer) - 1;
		int i = atoi(argv[1]) + 1;
		for(i; i > 0 && line >= 0; i--){
			fseek(file, line*sizeof(buffer), SEEK_SET);
			time(&now);
			fread(&buffer, sizeof(buffer), 1, file);
			printutmp(&buffer);
			line--;
		}
		printstart(file);
		fclose(file);
	}
	else{
		printf("invalid arguments\n");
		exit(1);
	}
	exit(0);
}
