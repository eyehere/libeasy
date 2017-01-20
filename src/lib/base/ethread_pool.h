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
#ifndef _ETHREAD_POOL_H_
#define _ETHREAD_POOL_H_
#include "ecommon.h"
#include "equeue.h"

#define THREAD_POOL_DATA(thread_context) thread_context->thread_pool->data
#define THREAD_POOL_QUEUE(thread_context) thread_context->thread_pool->queue
#define THREAD_ID(thread_context) thread_context->id

typedef void *(*thread_func_t)(void*);

typedef struct _thread_context_t thread_context_t;
typedef struct _thread_pool_t thread_pool_t;

struct _thread_context_t {
    int            id;
    pthread_t      thread;
    thread_pool_t *thread_pool;
    int            work_continue;
};

struct _thread_pool_t {
    thread_context_t *threads; 
    int               thread_num;
    queue_t          *queue;
    void             *data;
};

/** 
 *        Name: thread_pool_create 
 * Description: 创建线程池.
 *   Parameter: size -> 线程池内线程数量 
 *              thread_func_t -> 线程回调函数
 *              data -> 外部传入的数据
 *      Return: 线程池实例.
 */
thread_pool_t *thread_pool_create(int size, thread_func_t, void *data);

/** 
 *        Name: thread_pool_push_task 
 * Description: 向线程池push任务.
 *   Parameter: thread_pool -> 线程池实例.
 *              data -> 任务数据. 
 *              
 *      Return: 1 -> push成功
 *              0 -> push失败
 */
int thread_pool_push_task(thread_pool_t *thread_pool, void *data);

/** 
 *        Name: thread_pool_create 
 * Description: 创建线程池.
 *   Parameter: size -> 线程池内线程数量 
 *              thread_func_t -> 线程回调函数
 *      Return: return t1, t2's time delta, unit by ms.
 */
void thread_pool_destroy(thread_pool_t **thread_pool);

#endif
