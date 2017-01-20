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
#include "ethread_pool.h"

thread_pool_t *thread_pool_create(int size, thread_func_t func, void *data)
{
    int i = 0;
    thread_pool_t *thread_pool = NULL;

    if(size <= 0){
        return NULL;
    }
   
    thread_pool = (thread_pool_t *)MALLOC_WRAPPER(sizeof(thread_pool_t));
    if(NULL == thread_pool){
        printf("MALLOC_WRAPPER for thread pool failed.\n");
        return NULL;
    }

    thread_pool->threads = (thread_context_t *)MALLOC_WRAPPER(sizeof(thread_context_t) * size); 
    if(NULL == thread_pool->threads){
        printf("MALLOC_WRAPPER for thread_pool->threads failed.\n");
        thread_pool_destroy(&thread_pool);
        return NULL;
    }
    bzero(thread_pool->threads, sizeof(thread_context_t) * size);
    thread_pool->thread_num = size;

    thread_pool->queue = queue_create(1024);
    if(NULL == thread_pool->queue){
        printf("queue_create for thread_pool->queue failed.\n");
        thread_pool_destroy(&thread_pool);
        return NULL;
    }

    thread_pool->data = data;

    for(i = 0; i < size; i++){
        thread_pool->threads[i].id = i;
        thread_pool->threads[i].thread_pool = thread_pool;
        thread_pool->threads[i].work_continue = 1;
        if(0 != pthread_create(&thread_pool->threads[i].thread, 
                               NULL, func, &thread_pool->threads[i])){
            printf("create thread[%d] for thread pool failed.\n", i);
            thread_pool_destroy(&thread_pool);
            return NULL;
        }
    }
    
    return thread_pool;
}

int thread_pool_push_task(thread_pool_t *thread_pool, void *data)
{
    if(thread_pool == NULL){
        printf("thread_pool is NULL.\n");
        return 0;
    }

    return queue_push(thread_pool->queue, data);
}

void thread_pool_destroy(thread_pool_t **thread_pool)
{
    int i = 0;
    thread_pool_t *pool = NULL;

    if(NULL == thread_pool || NULL == *thread_pool){
        return;
    }

    pool = *thread_pool;
    if(pool->threads){
        for(i = 0; i < pool->thread_num; i++){
            pool->threads[i].work_continue = 0;
        }
        for(i = 0; i < pool->thread_num; i++){
            pthread_join(pool->threads[i].thread, NULL);
        }
        free(pool->threads);
        pool->threads = NULL;
    }

    if(pool->queue){
        queue_destroy(&pool->queue);
    }

    free(pool);
    *thread_pool = NULL;
}
