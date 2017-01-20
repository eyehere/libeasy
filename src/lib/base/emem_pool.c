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
#include "emem_pool.h"

mem_pool_t *mem_pool_create()
{
    void       *addr      = NULL;
    mem_pool_t *mem_pool  = NULL;
    int         page_size = 0;
    mem_page_t *page      = NULL;

    /* 获取内存page的大小 */ 
    page_size = getpagesize();
    
    /* 分配物理内存page */
    if(posix_memalign(&addr, page_size, page_size) != 0){
        printf("posix_memalign in mem_pool_create fails.\n");
        goto error;
    }

    /* 内存池信息和第一个page的信息都存储在第一个page的头部 */
    mem_pool             = (mem_pool_t *)addr;
    mem_pool->page_size  = page_size;
    mem_pool->page_count = 1;
    page                 = (mem_page_t *)((char *)addr + sizeof(mem_pool_t));
    mem_pool->current    = page;
    mem_pool->head       = page;
    mem_pool->tail       = page;

#ifdef MEM_POOL_LOCK
    pthread_mutex_init(&mem_pool->lock, NULL);
#endif

    /* 设置第一个page的信息 */
    page->start  = (char *)addr;
    page->last   = page->start + 
                   sizeof(mem_pool_t) + 
                   sizeof(mem_page_t);
    page->end    = (char *)addr + page_size;
    page->next   = NULL;
    page->failed = 0;

    return mem_pool;

error:
    if(mem_pool){
        mem_pool_destroy(&mem_pool);
    }
    return NULL;
}

void *mem_pool_alloc(mem_pool_t *mem_pool, int size)
{
    char       *ptr      = NULL;
    mem_page_t *current  = NULL; 
    mem_page_t *new_page = NULL;
    void       *addr     = NULL;

    if(!mem_pool){
        printf("mem_pool is null.\n");
        return NULL;
    }

    /* 先判断size的大小是否超出了page size */
    if(size > mem_pool->page_size - (int)sizeof(mem_page_t)){
        printf("size > %lu.\n", mem_pool->page_size - sizeof(mem_page_t));
        return NULL;
    }

#ifdef MEM_POOL_LOCK 
    pthread_mutex_lock(&mem_pool->lock);
#endif

    /* 从现有的page链表中寻找空间分配 */
    current = mem_pool->current;
    while(current){
        ptr = ALIGN_PTR(current->last, PLATFORM_ALIGNMENT);

        /* 如果空间足够 */
        if((char *)current->end - ptr >= size){
            current->last = ptr + size;

#ifdef MEM_POOL_LOCK
            pthread_mutex_unlock(&mem_pool->lock);
#endif
            return ptr;
        }
        
        current->failed++;
        if(current->failed >= MAX_FAILED_COUNT){
            mem_pool->current = current->next;
        }

        current = current->next; 
    }

    /* 现有的page空间没有足够的空间分配，必须创建新的page */
    if(posix_memalign(&addr, mem_pool->page_size, mem_pool->page_size) != 0){
        printf("posix_memalign in mem_pool_alloc fails.\n");

#ifdef MEM_POOL_LOCK
        pthread_mutex_unlock(&mem_pool->lock);
#endif
        return NULL;
    }

    new_page         = (mem_page_t *)addr;
    new_page->start  = (char *)addr;
    new_page->last   = new_page->start + sizeof(mem_page_t);
    new_page->end    = (char *)addr + mem_pool->page_size;
    new_page->next   = NULL;
    new_page->failed = 0;
    mem_pool->page_count++;

    /* 将新增加的page添加到page的链表的末尾 */
    mem_pool->tail->next = new_page;
    mem_pool->tail       = new_page;

    if(NULL == mem_pool->current){
        mem_pool->current = new_page;
    }

    ptr = ALIGN_PTR(new_page->last, PLATFORM_ALIGNMENT);
    new_page->last = ptr + size;

#ifdef MEM_POOL_LOCK
    pthread_mutex_unlock(&mem_pool->lock);
#endif

    return ptr;
}

void *mem_pool_calloc(mem_pool_t *mem_pool, int size)
{
    void *addr = NULL;

    addr = mem_pool_alloc(mem_pool, size);
    if(NULL == addr){
        return NULL;
    }
    bzero(addr, size);
    return addr;
}

void mem_pool_destroy(mem_pool_t **mem_pool)
{
    mem_pool_t *pool    = NULL;
    mem_page_t *current = NULL;

    if(NULL == mem_pool || NULL == *mem_pool){
        return;
    }

    pool = *mem_pool;
    current = pool->head;
    while(current){
        mem_page_t *page = current->start;
        current = current->next;
        free(page);  
    }

    *mem_pool = NULL;
}

void mem_pool_dump(mem_pool_t *mem_pool){
    int i = 0;
    mem_page_t *current = NULL;

    if(NULL == mem_pool){
        return;
    }

    printf("memory pool status information：\n");
    printf("page count: %d\n", mem_pool->page_count);

    current = mem_pool->head;
    while(current){
        printf("page: %d\n", i);
        printf("failed count: %d\n", current->failed);
        printf("page remained bytes: %lu\n", current->end - current->last);

        current = current->next;
        i++;
    }
}
