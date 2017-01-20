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
#ifndef _ESHM_H_
#define _ESHM_H_
#include "ecommon.h"
#include "elog.h"

typedef struct _shm_t {
    key_t  key;
    int    id;
    void  *base;
    size_t size;
    int    is_new; /* 表明共享内存是否本次创建，1 -> 本次创建，0 -> 打开已经存在的 */
    int    detached;
    int    deleted; 
}shm_t;

/** 
 *        Name: shm_create
 * Description: 创建一个共享内存实例.
 *   Parameter: shm_file -> 用来获取共享内存id的文件.
 *              size -> 共享内存的大小.
 *      Return: !NULL -> 返回共享内存实例.  
 *              NULL  -> 创建失败.
 */
shm_t *shm_create(char *shm_file, size_t size);

/** 
 *        Name: shm_detach
 * Description: 将共享内存从进程的逻辑地址空间脱离。 
 *   Parameter: shm -> 共享内存实例 
 *      Return: 1 -> 调用成功.  
 *              0 -> 调用失败.
 */
int shm_detach(shm_t *shm);

/** 
 *        Name: shm_delete
 * Description: 删除共享内存资源，但不释放shm的空间 
 *   Parameter: shm -> 共享内存实例 
 *      Return: 1 -> 调用成功.  
 *              0 -> 调用失败.
 */
int shm_delete(shm_t **shm);

/** 
 *        Name: shm_destroy
 * Description: 释放shm空间 
 *   Parameter: shm -> 共享内存实例 
 *      Return: 1 -> 调用成功.  
 *              0 -> 调用失败.
 */
int shm_destroy(shm_t **shm);
#endif
