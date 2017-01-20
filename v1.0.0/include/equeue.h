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
#ifndef _EQUEUE_H_
#define _EQUEUE_H_
#include "ecommon.h"
#include "ememory.h"

#define INIT_QUEUE_SIZE 1024
#define MAX_QUEUE_SIZE  (1024 * 1024 * 4)

typedef struct _queue_t{
    void **q;      /* 队列节点数组 */
    int    front;  /* 队列头部索引 */
    int    rear;   /* 队列尾部索引 */
    int    size;   /* 队列能够容纳的节点数 */

    pthread_mutex_t lock; /* 队列操作锁 */
}queue_t;

/** 
 *        Name: queue_create
 * Description: 创建队列.
 *   Parameter: size -> 队列size, size为0时，设置成默认值.
 *      Return: 返回创建的队列实例.
 */
queue_t *queue_create(int size);

/** 
 *        Name: queue_destroy
 * Description: 销毁队列.
 *   Parameter: queue -> 队列实例. 
 *      Return: 
 */
void queue_destroy(queue_t **queue);

/** 
 *        Name: queue_push 
 * Description: 进队列.
 *   Parameter: queue -> 队列实例 
 *              data -> 队列节点数据 
 *      Return: 进队列是否成功.
 */
int queue_push(queue_t *queue, void *data);

/** 
 *        Name: queue_pop
 * Description: 出队列.
 *   Parameter: queue -> 队列实例
 *      Return: 返回出队节点数据.
 */
void *queue_pop(queue_t *queue);

/** 
 *        Name: queue_head
 * Description: 查看头部元素.
 *   Parameter: queue -> 队列实例
 *      Return: 得到头部数据.
 */
void *queue_head(queue_t *queue);

/** 
 *        Name: queue_empty
 * Description: 队列是否为空
 *   Parameter: queue -> 队列实例 
 *      Return: 1 -> 队列为空.  
 *              0 -> 队列不为空
 */
int queue_empty(queue_t *queue);

/** 
 *        Name: queue_element_count
 * Description: 队列中元素的个数.
 *   Parameter: queue -> 队列实例. 
 *      Return: 队列元素个数.  
 */
int queue_element_count(queue_t *queue);

/** 
 *        Name: queue_size
 * Description: 返回队列能够容纳的节点数量.
 *   Parameter: queue -> 队列实例. 
 *      Return: 返回队列能够容纳的节点数量.  
 */
int queue_size(queue_t *queue);

#endif
