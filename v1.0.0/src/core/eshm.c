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
#include "eshm.h"

shm_t *shm_create(char *shm_file, size_t size)
{
    shm_t *shm = NULL;

    if(size <= 0){
        log_error("size <= 0 in shm_create.");
        return NULL;
    }

    shm = (shm_t *)malloc(sizeof(shm_t));
    if(NULL == shm){
        log_error("malloc failed, %s.", strerror(errno));
        goto error;
    }
    bzero(shm, sizeof(shm_t));

    shm->key = ftok(shm_file, 0);
    if(-1 == shm->key){
        log_error("ftok for shm_create fails, %s is not existed.", shm_file);
        goto error;
    }

    shm->size   = size;
    shm->is_new = 1;

    /* 先判断共享内存是否存在 */
    shm->id = shmget(shm->key, shm->size, IPC_CREAT|IPC_EXCL);
    if(-1 == shm->id){
        if(EEXIST != errno){
            log_error("shmget failed, %s.", strerror(errno));
            goto error;
        }

        /* 共享内存已经存在 */
        shm->is_new = 0;
        shm->id = shmget(shm->key, shm->size, IPC_CREAT);
        if(-1 == shm->id){
            log_error("shmget failed, %s.", strerror(errno));
            goto error;
        }
    }
    shm->deleted = 0;

    /* 将共享内存关联到进程逻辑地址空间 */
    shm->base = shmat(shm->id, NULL, 0);
    if((void *)-1 == shm->base){
        log_error("shmat failed, %s.", strerror(errno));
        goto error;
    }
    shm->detached = 0;

    return shm;

error:
    shm_destroy(&shm); 
    return NULL;
}

int shm_detach(shm_t *shm)
{
    if(NULL == shm){
        return 1;
    }

    if(shm->detached){
        return 1;
    }

    if(shm->base){
        if(-1 == shmdt(shm->base)){
            log_error("shmdt failed, %s.", strerror(errno));
            return 0;
        }
    }

    shm->base = NULL;
    shm->detached = 1;
    return 1;
}

int shm_delete(shm_t **shm)
{
    if(NULL == shm || NULL == *shm){
        return 1;
    }

    if((*shm)->deleted){
        return 1;
    }

    /* 如果没有detach */
    if(!(*shm)->detached){
        shm_detach(*shm);
        (*shm)->detached = 1;
    }

    if(-1 == shmctl((*shm)->id, IPC_RMID, NULL)){
        log_error("shmctl failed, %s.", strerror(errno));
        return 0;
    }

    (*shm)->deleted = 1;

    shm_destroy(shm);
    return 1;
}

int shm_destroy(shm_t **shm)
{
    shm_t *p = NULL; 

    if(NULL == shm || NULL == *shm){
        return 1;
    }

    p = *shm;

    if(p->base){
        shm_detach(p);
    }

    free(p);
    *shm = NULL;

    return 1;
}
