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
#ifndef _EOTHER_H_
#define _EOTHER_H_
#include "ecommon.h"

/** 
 *        Name: file_suffix
 * Description: get file name's suffix.
 *              for example: xxxxxx.mpg
 *              return .mpg.
 *   Parameter: file_name -> file name.
 *      Return: file name's suffix.
 */
char *file_suffix(char *file_name);

/** 
 *        Name: file_get_length
 * Description: get file's length.
 *   Parameter: file_name -> file name.
 *      Return: file length.
 */
off_t file_get_length(const char *file_name);

/** 
 *        Name: create_random
 * Description: generate a random number between 0 ~ max.
 *   Parameter: max -> random number <= max.
 *              
 *      Return: return a random number between 0 ~ max.
 */
int create_random(int max);

/** 
 *        Name: list_dir 
 * Description: list first level sub directory.
 *   Parameter: path -> directory path.
 *              callback -> every file in the directory, call callback. 
 *      Return: 1 -> list successflly.
 *              0 -> list failedly.
 */
int list_dir(char *path, void (*callback)(char *file_name)); 

/** 
 *        Name: create_dir 
 * Description: create directory, if directory exists, return.
 *   Parameter: path_name -> directory name.
 *              
 *      Return: 1 -> create successfully or directory exist.
 *              0 -> create failedly
 */
int create_dir(char *path_name);

/** 
 *        Name: file_exist
 * Description: 判断文件是否存在.
 *   Parameter: file_name -> file path
 *              
 *      Return: 1 -> 文件是否存在。 
 *              0 -> 文件不存在。
 */
int file_exist(char *file_name);

/** 
 *        Name: create_file
 * Description: 创建文件.
 *   Parameter: file_name -> file path
 *              size -> 文件大小
 *              
 *      Return: 1 -> 创建文件成功。 
 *              0 -> 创建文件失败。
 */
int create_file(char *file_name, size_t size);

/** 
 *        Name: create_file_if_not_existed
 * Description: 如果文件不存在,创建该文件.
 *   Parameter: file_name -> 文件名.
 *              
 *      Return: 1 -> 创建成功.
 *              0 -> 创建失败. 
 */
int create_file_if_not_existed(char *file_name);

/** 
 *        Name: remove_file
 * Description: 如果文件存在，删除它.
 *   Parameter: file_name -> 文件名.
 *              
 *      Return: 1 -> 删除成功.
 *              0 -> 删除失败. 
 */
int remove_file(char *file_name);

/**
 *        Name: get_ip
 * Description: 获取本机的IP
 *   Parameter: version -> v4 | v6.
 *	 Parameter: address ->
 *	 				char address[INET_ADDRSTRLEN] |
 *	 				char address[INET6_ADDRSTRLEN]
 *      Return: 1 -> 成功
 *      		0 -> 失败
 */
int get_ip(const char* version, char* address);

#endif
