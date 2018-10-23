#ifndef RRU_LOG_H_
#define RRU_LOG_H_

#include <stdio.h>

#define DEBUG	 	0
#define INFO 		1
#define WARN  		2
#define ERROR 		3
#define FATAL 		4

#define PRINT_SCREEN    2
#define OUTPUT_FILE     1

int config_log(char * path, unsigned int filter, unsigned mode);
void logger(int level, char *fname, int line_number,const char *format,...);

#define LOG_WARN(message,...) logger(WARN,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_DEBUG(message,...) logger(DEBUG,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_INFO(message,...) logger(INFO,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_ERROR(message,...) logger(ERROR,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_FATAL(message,...) logger(FATAL,__FILE__,__LINE__,message,##__VA_ARGS__)
#endif /* RRU_LOG_H_ */