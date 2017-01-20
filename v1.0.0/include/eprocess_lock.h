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
#ifndef _EPROCESS_LOCK_H_
#define _EPROCESS_LOCK_H_
#include "ecommon.h"

typedef struct _process_lock_t {
    key_t key; 
    int   id;
}process_lock_t;

/** 
 *        Name: process_lock_create
 * Description: 创建进程间同步的互斥锁。
 *   Parameter: file   -> 用来生成互斥锁key的文件。
 *              sub_id -> 和file一起生成互斥锁key。
 *             
 *      Return: 返回互斥锁，失败返回。
 */
process_lock_t *process_lock_create(char *file, int sub_id);

/** 
 *        Name: process_lock_init
 * Description: 初始化进程间同步的互斥锁, 
 *              不同的进程之间只能调用一次。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 初始化成功。
 *              0 -> 初始化失败。
 */
int process_lock_init(process_lock_t *pm);

/** 
 *        Name: process_try_lock
 * Description: 试图执行锁操作，如果成功则锁住。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1  -> 锁成功。
 *              0  -> 没有锁住。
 *              -1 -> 锁失败。
 */
int process_try_lock(process_lock_t *pm);

/** 
 *        Name: process_lock
 * Description: 执行锁操作。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 锁成功。
 *              0 -> 锁失败。
 */
int process_lock(process_lock_t *pm);

/** 
 *        Name: process_unlock
 * Description: 执行解锁操作。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 解锁成功。
 *              0 -> 解锁失败。
 */
int process_unlock(process_lock_t *pm);

/** 
 *        Name: process_lock_release
 * Description: 销毁锁实例,但并不删除全局的信号灯。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 销毁锁实例成功。
 *              0 -> 销毁锁实例失败。
 */
int process_lock_release(process_lock_t **pm);

/** 
 *        Name: process_lock_delete
 * Description: 删除全局的信号灯，应该在process_lock_release之前调用。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 删除全局信号灯成功。
 *              0 -> 删除全局信号灯失败。
 */
int process_lock_delete(process_lock_t *pm);

#endif
