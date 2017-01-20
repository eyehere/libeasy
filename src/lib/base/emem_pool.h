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
#ifndef _EMEM_POOL_H_
#define _EMEM_POOL_H_
#include "ecommon.h"

#define MAX_FAILED_COUNT   4
#define PLATFORM_ALIGNMENT sizeof(unsigned long)

/* 内存池初始化时，只有一个page单元, 以后自动扩展
 * 但扩容的单位仍然是一个page */
typedef struct _mem_page_t{
    void               *start;     /* 内存page的起始地址 */
    void               *last;      /* 内存page可用内存的地址 */
    void               *end;       /* 内存page的末地址 */
    struct _mem_page_t *next;      /* 下一个内存page */
    int                 failed;    /* 当前page申请内存失败的次数 */
}mem_page_t;

typedef struct _mem_pool_t {
    int         page_size;         /* 内存page的大小，为固定值 */
    int         page_count;        /* 当前内存池有多少个page */
    mem_page_t *current;           /* 当前正在使用的page */
    mem_page_t *head;              /* 头page */
    mem_page_t *tail;              /* 尾page */

#ifdef MEM_POOL_LOCK
    pthread_mutex_t lock;
#endif
}mem_pool_t;

/** 
 *        Name: mem_pool_create
 * Description: 创建内存池.
 *   Parameter:  
 *      Return: 返回内存池实例.
 */
mem_pool_t *mem_pool_create();

/** 
 *        Name: mem_pool_alloc
 * Description: 从内存池分配内存，不初始化(清0).
 *   Parameter:  
 *      Return: NULL  -> 分配失败.
 *              !NULL -> 分配成功.
 */
void *mem_pool_alloc(mem_pool_t *mem_pool, int size);

/** 
 *        Name: mem_pool_calloc
 * Description: 从内存池分配内存，初始化(清0).
 *   Parameter:  
 *      Return: NULL  -> 分配失败.
 *              !NULL -> 分配成功.
 */
void *mem_pool_calloc(mem_pool_t *mem_pool, int size);

/** 
 *        Name: mem_pool_destroy
 * Description: 销毁内存池.
 *   Parameter: mem_pool -> 内存池实例的地址. 
 *      Return: 
 *              
 */
void mem_pool_destroy(mem_pool_t **mem_pool);

/** 
 *        Name: mem_pool_dump
 * Description: dump的状态信息.
 *   Parameter: mem_pool -> 内存池实例的地址. 
 *      Return: 
 *              
 */
void mem_pool_dump(mem_pool_t *mem_pool);
#endif
