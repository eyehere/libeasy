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
#ifndef _ESTRING_H_
#define _ESTRING_H_
#include "ecommon.h"

/* 用来表示内存 */
typedef struct _string_t {
    char *data;
    int   len;
}string_t;

/** 
 *        Name: is_comment
 * Description: judge whether string is comment.
 *   Parameter: str -> string.
 *              comment_char -> comment char, for example '#'.
 *      Return: 1 -> comment string.  
 *              0 -> not comment string.
 */
int is_comment(char *str, char comment_char);

/** 
 *        Name: is_blank
 * Description: judge whether string is blank.
 *   Parameter: str -> string.
 *      Return: 1 -> blank line.
 *              0 -> not blank line.
 */
int is_blank(char *str);

/** 
 *        Name: trim_str 
 * Description: trim string.
 *   Parameter: str -> string.
 *      Return: return trimed string.
 */
char* trim_str(char *str);

/** 
 *        Name: delete_comment. 
 * Description: delete comment part from current string.
 *   Parameter: str -> string.
 *      Return: 
 */ 
void delete_comment(char *str, char comment_char);

/** 
 *        Name: char_at. 
 * Description: loacte specified char.
 *   Parameter: str -> string.
 *              c   -> located char.
 *      Return: the index of located char. 
 */
int char_at(char *str, char c);

/** 
 *        Name: split_url. 
 * Description: split url into two part: path and name.
 *              example1:
 *              url  -> http://localhost/media/0.m3u8
 *              path -> http://localhost/media/
 *              name -> 0.m3u8
 *
 *              example 2:
 *              url  -> /var/log/test.log
 *              path -> /var/log/
 *              name -> test.log
 *
 *   Parameter: url  -> string.
 *              path -> url path part.
 *              path_size -> path size.
 *              name -> url name part.
 *              name_size -> name size.
 *      Return: 
 */
void split_url(char *url, char *path, int path_size, char *name, int name_size);

/** 
 *        Name: str_cmp
 * Description: compare two string.
 *   Parameter: str1  -> string 1.
 *              size1 -> string 1 size.
 *              str2  -> string 2.
 *              size2 -> string 2 size.
 *      Return: str1 == str2, return 0
 *              str1 > str2, return 1 
 *              str1 < str2, return -1
 */
int str_cmp(char *str1, int size1, char *str2, int size2);

#endif

