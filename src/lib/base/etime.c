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
#include "etime.h"

int time_delta(struct timeval *t1, struct timeval *t2){
    int delta;

    delta = (t1->tv_sec - t2->tv_sec) * 1000 + 
        (t1->tv_usec - t2->tv_usec)/1000;
    if(delta < 0){
        delta = -delta;
    }

    return delta;
}

int time_signed_delta(struct timeval *t1, struct timeval *t2){
    int delta;

    delta = (t1->tv_sec - t2->tv_sec) * 1000 + 
        (t1->tv_usec - t2->tv_usec)/1000;

    return delta;
}

int time_great(struct timeval *t1, struct timeval *t2){
    if(t1->tv_sec > t2->tv_sec){
        return 1;
    }
    else if(t1->tv_sec  == t2->tv_sec){
        if(t1->tv_usec > t2->tv_usec){
            return 1;
        }
        else if(t1->tv_usec == t2->tv_usec){
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
}

int time_less(struct timeval *t1, struct timeval *t2){
    return time_great(t2, t1);
}

void time_add(struct timeval *t1, int increment){
    int64_t t   = t1->tv_sec * 1000 + t1->tv_usec / 1000;
    t           += increment;
    t1->tv_sec  = t/1000;
    t1->tv_usec = t%1000 * 1000; 
}

void time_subtract(struct timeval *t1, int decrement){
    int64_t t   = t1->tv_sec * 1000 + t1->tv_usec/1000;
    t           -= decrement;
    t1->tv_sec  = t/1000;
    t1->tv_usec = t%1000 * 1000;
}

void time_str(char *time_buffer, int buffer_size){
    time_t t = 0;

    t = time(0);

    bzero(time_buffer, buffer_size);
    strftime(time_buffer, buffer_size, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

void time_str_by_time(char *time_buffer, int buffer_size, time_t t){
    bzero(time_buffer, buffer_size);
    strftime(time_buffer, buffer_size, "%Y-%m-%d %H:%M:%S", localtime(&t));
}
