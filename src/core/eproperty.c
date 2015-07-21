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
#include "eproperty.h"
#include "estring.h"

static char _comment_char = '#';

static int (*p_item_process_cb)(char *key, char *value, void *userp);

static int default_item_process_cb(char *key, char *value, void *userp){
    printf("%-30s  %s\n", key, value); 
    return 1;
}

int property_read(char *file_name, int (*item_process_cb)(char *key, char *value, void *userp), 
                  void *userp){
    FILE *fp = NULL;
    char buffer[2048] = {0};
    char *line = NULL;
    int result = 0;

    if(item_process_cb){
        p_item_process_cb = item_process_cb;
    } 
    else{
        p_item_process_cb = default_item_process_cb;
    }

    if(NULL == file_name){
        printf("file_name is NULL in config_read!\n");
        return 0;
    } 

    if(NULL == (fp = fopen(file_name, "r"))){
        printf("config_load:fopen %s failed!\n", file_name); 
        return 0;
    }

    while(!feof(fp)){
        char *key = NULL;
        char *value = NULL;
        int offset = 0;
        bzero(buffer, sizeof(buffer)); 
        line = fgets(buffer, sizeof(buffer), fp); 
        if(NULL == line){
            continue;
        }

        if(is_blank(line)){
            continue;
        }

        if (_comment_char != 0) {
            if(is_comment(line, _comment_char)){
                continue; 
            }

            delete_comment(line, _comment_char); 
        }

        line = trim_str(line);
        offset = char_at(line, '=');
        line[offset] = 0;
        key = line;
        value = trim_str(line + offset + 1);
        key = trim_str(key); 
        result = p_item_process_cb(key, value, userp);
        if(!result){
            break;
        }
    }
    
    fclose(fp); 
    return result;
}

void property_set_comment_char(char c)
{
    _comment_char = c;
}
