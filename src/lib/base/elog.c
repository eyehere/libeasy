/*
  +----------------------------------------------------------------------+
  | libeasy                                                              |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | yiming_6weijun@163.com so we can mail you a copy immediately.        |
  +----------------------------------------------------------------------+
  | Author: Weijun Lu  <yiming_6weijun@163.com>                          |
  +----------------------------------------------------------------------+
*/
#include "elog.h"

#define LOG_DESTINATION_CONSOLE 0
#define LOG_DESTINATION_FILE    1
#define LOG_DESTINATION_SYSLOG  2

#define LINE_BUFFER_SIZE   8096
#define FORMAT_BUFFER_SIZE 1024

static int log_level = LOG_LEVEL_INFO;
static int log_fd    = STDOUT_FILENO;
static int log_dst   = LOG_DESTINATION_CONSOLE;

static char *log_level_map[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

static int syslog_level_map[] = {
    SYSLOG_DEBUG,
    SYSLOG_INFO,
    SYSLOG_WARNING,
    SYSLOG_ERR
};

static void format_cur_time(char *time_buffer, int time_buffer_size, char *format);

int log_set_file(char *file_name)
{
    int fd = -1;

    if (log_fd != STDOUT_FILENO) {
        close(log_fd);
    }

    fd = open(file_name, O_WRONLY|O_APPEND|O_CREAT, 0644);
    if(-1 == fd){
        printf("open %s failed, %s\n", file_name, strerror(errno));
        return 0;
    }

    log_dst = LOG_DESTINATION_FILE;
    log_fd = fd;
    return 1; 
}

int log_set_syslog(char *ip, int port, int facility, char *hostname, char *program)
{
    int result = 0;
    result = syslog_open(ip, port, facility, hostname, program);
    if(result){
        log_dst = LOG_DESTINATION_SYSLOG;
    }

    return result;
}

int log_close()
{
    if(LOG_DESTINATION_FILE == log_dst ||
       LOG_DESTINATION_CONSOLE == log_dst){
        if(log_fd >= 3){
            close(log_fd);
            log_fd = STDOUT_FILENO;
        }
    }
    else if(LOG_DESTINATION_SYSLOG == log_dst){
        syslog_close();
    }
    else{
        printf("unknown log_dst == %d.\n", log_dst);
    }
    return 1;
}

void log_set_level(int level)
{
    if(log_level < LOG_LEVEL_DEBUG){
        log_level = LOG_LEVEL_DEBUG; 
        return ;
    } 

    if(log_level > LOG_LEVEL_ERROR){
        log_level = LOG_LEVEL_ERROR;
        return ;
    }
    log_level = level;
}

int log_by_level(int level, const char *fmt, ...)
{
    va_list vl;
    char time_buffer[32]    = {0};
    char line_buffer[LINE_BUFFER_SIZE]  = {0};
    char format_buffer[FORMAT_BUFFER_SIZE] = {0};
    int  line_size          = 0;

    if(level < log_level)
        return 1;
    
    if(level > LOG_LEVEL_ERROR){
        level = LOG_LEVEL_ERROR;
    }

    if(LOG_DESTINATION_SYSLOG != log_dst){
        format_cur_time(time_buffer, sizeof(time_buffer), NULL);
        snprintf(format_buffer, sizeof(format_buffer) - 1, "[%s] [%s] %s", 
                log_level_map[level], time_buffer, fmt);

        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, format_buffer, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;
        write(log_fd, line_buffer, line_size);
    }
    else{
        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, fmt, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;

        if(!syslog_write(syslog_level_map[level], line_buffer, line_size)){
            return 0;
        }
    }
    
    return 1;
}

int log_debug(const char *fmt, ...)
{
    va_list vl;
    char time_buffer[32]    = {0};
    char line_buffer[LINE_BUFFER_SIZE]  = {0};
    char format_buffer[FORMAT_BUFFER_SIZE] = {0};
    int  line_size          = 0;
    int  level              = LOG_LEVEL_DEBUG;

    if(level < log_level)
        return 1;
    
    if(level > LOG_LEVEL_ERROR){
        level = LOG_LEVEL_ERROR;
    }

    if(LOG_DESTINATION_SYSLOG != log_dst){
        format_cur_time(time_buffer, sizeof(time_buffer), NULL);
        snprintf(format_buffer, sizeof(format_buffer) - 1, "[%s] [%s] %s", 
                log_level_map[level], time_buffer, fmt);

        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, format_buffer, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;
        write(log_fd, line_buffer, line_size);
    }
    else{
        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, fmt, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;

        if(!syslog_write(syslog_level_map[level], line_buffer, line_size)){
            return 0;
        }
    }
    return 1;
}

int log_info(const char *fmt, ...)
{
    va_list vl;
    char time_buffer[32]    = {0};
    char line_buffer[LINE_BUFFER_SIZE]  = {0};
    char format_buffer[FORMAT_BUFFER_SIZE] = {0};
    int  line_size          = 0;
    int  level              = LOG_LEVEL_INFO;

    if(level < log_level)
        return 1;
    
    if(level > LOG_LEVEL_ERROR){
        level = LOG_LEVEL_ERROR;
    }

    if(LOG_DESTINATION_SYSLOG != log_dst){
        format_cur_time(time_buffer, sizeof(time_buffer), NULL);
        snprintf(format_buffer, sizeof(format_buffer) - 1, "[%s] [%s] %s", 
                log_level_map[level], time_buffer, fmt);

        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, format_buffer, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;
        write(log_fd, line_buffer, line_size);
    }
    else{
        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, fmt, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;

        if(!syslog_write(syslog_level_map[level], line_buffer, line_size)){
            return 0;
        }
    }
    return 1;
}

int log_warn(const char *fmt, ...)
{
    va_list vl;
    char time_buffer[32]    = {0};
    char line_buffer[LINE_BUFFER_SIZE]  = {0};
    char format_buffer[FORMAT_BUFFER_SIZE] = {0};
    int  line_size          = 0;
    int  level              = LOG_LEVEL_WARN;

    if(level < log_level)
        return 1;
    
    if(level > LOG_LEVEL_ERROR){
        level = LOG_LEVEL_ERROR;
    }

    if(LOG_DESTINATION_SYSLOG != log_dst){
        format_cur_time(time_buffer, sizeof(time_buffer), NULL);
        snprintf(format_buffer, sizeof(format_buffer) - 1, "[%s] [%s] %s", 
                log_level_map[level], time_buffer, fmt);

        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, format_buffer, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;
        write(log_fd, line_buffer, line_size);
    }
    else{
        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, fmt, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;

        if(!syslog_write(syslog_level_map[level], line_buffer, line_size)){
            return 0;
        }
    }
    return 1;
}

int log_error(const char *fmt, ...)
{
    va_list vl;
    char time_buffer[32]    = {0};
    char line_buffer[LINE_BUFFER_SIZE]  = {0};
    char format_buffer[FORMAT_BUFFER_SIZE] = {0};
    int  line_size          = 0;
    int  level              = LOG_LEVEL_ERROR;

    if(level < log_level)
        return 1;
    
    if(level > LOG_LEVEL_ERROR){
        level = LOG_LEVEL_ERROR;
    }

    if(LOG_DESTINATION_SYSLOG != log_dst){
        format_cur_time(time_buffer, sizeof(time_buffer), NULL);
        snprintf(format_buffer, sizeof(format_buffer) - 1, "[%s] [%s] %s", 
                log_level_map[level], time_buffer, fmt);

        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, format_buffer, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;
        write(log_fd, line_buffer, line_size);
    }
    else{
        va_start(vl, fmt);
        line_size = vsnprintf(line_buffer, sizeof(line_buffer) - 2, fmt, vl);
        if(line_size <= 0){
            return 0;
        }
        va_end(vl);

        if ((unsigned int)line_size > sizeof(line_buffer) - 2) {
            line_size = sizeof(line_buffer) - 2;
        }
        line_buffer[line_size] = '\n';
        line_size++;

        if(!syslog_write(syslog_level_map[level], line_buffer, line_size)){
            return 0;
        }
    }
    return 1;
}

char *log_level_str(int level){
    if(level < 0 || level > 3){
        log_error("log_level should be 0, 1, 2, 3, %d is illegal\n", level);
        return NULL;
    }
    return log_level_map[level];
}

int log_level_int(char *level_str){
    if(0 == strcasecmp(level_str, "debug")){
        return LOG_LEVEL_DEBUG;
    }
    else if(0 == strcasecmp(level_str, "info")){
        return LOG_LEVEL_INFO;
    }
    else if(0 == strcasecmp(level_str, "warn")){
        return LOG_LEVEL_WARN;
    }  
    else if(0 == strcasecmp(level_str, "error")){
        return LOG_LEVEL_ERROR;
    }
    else{
        return -1;
    }
}

static void format_cur_time(char *time_buffer, int time_buffer_size, char *format){
    struct tm *ptm = NULL;
    time_t cur_time;

    cur_time = time(NULL);    
    ptm = localtime(&cur_time);

    if(NULL == format){
        strftime(time_buffer, time_buffer_size, "%Y-%m-%d %H:%M:%S", ptm);
    }   
    else{
        strftime(time_buffer, time_buffer_size, format, ptm);
    }   
}

#ifdef LOG_DEBUG
int main(void)
{
    int count = 100000;
    log_set_file("/var/va_server/test.log");
    log_set_level(LOG_LEVEL_DEBUG);

    if(fork() > 0){
        while(count-- > 0){
            log_debug("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
            log_info("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
            log_warn("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
            log_error("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        }
        log_close();
    }
    else{
        while(count-- > 0){
            log_debug("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
            log_info("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
            log_warn("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
            log_error("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        }
        log_close();
    }

    return 1;
}
#endif
