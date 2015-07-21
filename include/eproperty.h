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
#ifndef _EPROPERTY_H_
#define _EPROPERTY_H_
#include "ecommon.h"

/** 
 *        Name: property_read 
 * Description: read property file
 *   Parameter: file_name -> property file name.
 *              item_process_cb -> every item process callback.
 *              userp -> user data.
 *      Return: 1.  
 */
int property_read(char *file_name, int (*item_process_cb)(char *key, char *value, void *userp), 
                  void *userp);

/** 
 *        Name: property_set_comment_char
 * Description: 设置注释字符，默认#，如果c=0, 表示不处理注释行
 *   Parameter: c -> 注释字符.
 *
 *      Return: 
 */
void property_set_comment_char(char c);

#endif
