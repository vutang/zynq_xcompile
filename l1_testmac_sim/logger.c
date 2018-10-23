/*
* @Author: vutt6
* @Date:   2018-10-10 16:05:38
* @Last Modified by:   vutt6
* @Last Modified time: 2018-10-10 17:29:21
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include "logger.h"

#define MAX_MSG_SIZE_PER_LINE 300
#define MAX_LOG_FILE_SPACE 500000
#define MAX_ROTATE 2
#define MAX_PATH_LEN       80

static FILE *fd;
static unsigned int logDirection;
static unsigned int logFilter;
static unsigned int total_space_written;
static unsigned int rotate;
static char         name_path[MAX_PATH_LEN];

static char *levelName[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

int config_log(char * path, unsigned int filter, unsigned mode) { 
	fd = NULL;
	total_space_written = 0;
	rotate = MAX_ROTATE;
	logFilter = filter;
	logDirection = mode;
	strcpy(name_path,path); 

	if (logDirection & OUTPUT_FILE) {
		fd = fopen(path, "a");
		if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
		else {
			fseek(fd, 0, SEEK_END);
			total_space_written = ftell(fd);
			fseek(fd, 0 ,SEEK_SET);
		} 
	}
	return 0;
}

void roltate_log(void) {
	char file_name[MAX_PATH_LEN],old_name[MAX_PATH_LEN], new_name[MAX_PATH_LEN];
	int  i;
	sprintf(file_name, "%s", name_path);
	if (fd) {
		if (rotate > 0) {
			for ( i = rotate - 1; i > 0; i-- ) {
				sprintf(old_name, "%s.%d", name_path, i-1);
				sprintf(new_name, "%s.%d", name_path, i  );
				rename(old_name, new_name); 
			}
			sprintf(new_name,"%s.%d",name_path, 0);
			fflush(fd);
			fclose(fd);
			rename(file_name, new_name);
		} 
		else {
			fflush(fd);
			fclose(fd); 
		}	
		fd = fopen(file_name, "a");
		if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
	} 
	else { // ed == null
		fd = fopen(file_name,"a");
		if (fd) {
			fseek(fd, 0L, SEEK_END);
			total_space_written = ftell(fd);
		} 
		else {
			printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
		} 
	}
}

int write_log_msg(char *msg) {
	struct timeval  tv;
	struct tm      *tm;
	time_t ltime;
	unsigned int  write_size;

	/*Get time*/
	gettimeofday(&tv, NULL);
	ltime = time(NULL);
	tm = localtime(&ltime);

	/*Open log file*/
	if (!fd) {
		fd = fopen(name_path, "a");
		if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
	}
	if (fd) {
		write_size = fprintf(fd, "%04d-%02d-%02d %02d:%02d:%02d %s\n",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,msg);
		total_space_written += write_size;   
		fflush(fd);
	}
	if (total_space_written > MAX_LOG_FILE_SPACE) {
		roltate_log();
		total_space_written = 0;
	}
	return 0; 
}

void logger(int level, char *fname, int line_number, const char *format, ...) {
	va_list argptr;
	char msg[MAX_MSG_SIZE_PER_LINE];
	int len;

	if (logFilter & (1 << level)) {
		sprintf(msg, "%-7s %-20s #%-7d", levelName[level], fname, line_number);
		len = strlen(msg);
		va_start(argptr, format);
		// 30 is for date and time.
		vsnprintf(&msg[len], MAX_MSG_SIZE_PER_LINE-len-30, format, argptr);
		va_end(argptr);
		if (logDirection & PRINT_SCREEN ) {
			switch(level) {
				case DEBUG:  
					printf("\33[34m[D] %s\33[0m\n",msg);
					break;
				case INFO:
					printf("[J] %s\n",msg);
					break;
				case WARN:	
					printf("\33[33m[W] %s \33[0m\n",msg);
					break;
				default :
					printf("\33[31m[E] %s \33[0m\n",msg);
					break;
			};
		}
		if (logDirection & OUTPUT_FILE)
			write_log_msg(msg);
	}
}