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
#include "equeue.h"

static int queue_full(queue_t *queue);

queue_t *queue_create(int size)
{
    queue_t *queue = NULL;

    if(size < 0){
        printf("size < 0.\n");
        return NULL;
    }

    if(size == 0 || size == 1){
        size = INIT_QUEUE_SIZE;
    }

    queue = (queue_t *)malloc(sizeof(queue_t)); 
    if(NULL == queue){
        printf("malloc for queue failed.\n");
        return NULL;
    }
    bzero(queue, sizeof(queue_t));

    queue->q = (void **)malloc(sizeof(void *) * size);
    if(NULL == queue->q){
        printf("malloc for queue->q failed.\n");
        free(queue);
        return NULL;
    }

    queue->size = size;

    pthread_mutex_init(&queue->lock, NULL);

    return  queue;
}

void queue_destroy(queue_t **queue){
    if(NULL == queue || NULL == *queue){
        return;
    }
    
    if((*queue)->q){
        free((*queue)->q);
        (*queue)->q = NULL;
    }
    free(*queue);
    *queue = NULL;
    return;
}

int queue_push(queue_t *queue, void *data)
{
    int i        = 0;
    int org_size = 0;

    pthread_mutex_lock(&queue->lock);

    /* 如果队列已满，需要扩容， x2 */
    if(queue_full(queue)){
        org_size    = queue->size;
        queue->size = queue->size * 2;

        printf("queue 扩容 to %d\n", queue->size);
        queue->q = (void **)realloc(queue->q, sizeof(void *) * queue->size);
        if(NULL == queue->q){
            printf("realloc for queue->q failed.\n");
            pthread_mutex_unlock(&queue->lock);
            return 0;
        }

        /* 扩容成功后,整理队列 */
        if(queue->rear < queue->front){
            for(i = 0; i <= queue->rear; i++){
                queue->q[org_size + i] = queue->q[i];
            }
            queue->rear = org_size + queue->rear;
        }
    }

    /* 添加到队列尾部 */
    queue->rear = (queue->rear + 1) % queue->size;
    queue->q[queue->rear] = data;

    pthread_mutex_unlock(&queue->lock);
    return 1;
}

void *queue_pop(queue_t *queue)
{
    void *data = NULL;

    pthread_mutex_lock(&queue->lock);
    if(queue->front == queue->rear){
        pthread_mutex_unlock(&queue->lock);
        printf("queue is empty, delete?.\n");
        return NULL;
    }

    queue->front = (queue->front + 1) % queue->size;
    data = queue->q[queue->front]; 

    pthread_mutex_unlock(&queue->lock);
    return data;
}

void *queue_head(queue_t *queue) 
{
    void *data = NULL;

    pthread_mutex_lock(&queue->lock);
    int head = (queue->front + 1) % queue->size;
    data = queue->q[head];
    pthread_mutex_unlock(&queue->lock);

    return data;
}

int queue_empty(queue_t *queue)
{
    if(queue->front == queue->rear){
        return 1;
    }
    return 0;
}

int queue_element_count(queue_t *queue)
{
    return (queue->rear - queue->front + queue->size) % queue->size;
}

int queue_size(queue_t *queue)
{
    return queue->size;
}

/* ==================================静态内部函数=================================== */

static int queue_full(queue_t *queue){
    if((queue->rear + 1) % queue->size == queue->front){
        return 1;
    }
    return 0;
}
