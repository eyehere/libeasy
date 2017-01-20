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
#ifndef SERVER_CONHASH_H_
#define SERVER_CONHASH_H_
#include "easy.h"
#include "conhash.h"
#include "hashmap.h"

typedef struct _server_conhash_t {
    hashmap_t *conhashs; /* IDC一致性哈希实例哈希表 */
    hashmap_t *ips;  /* 存储haship到ip的映射 */
}server_conhash_t;

/** 
 *        Name: server_conhash_create
 * Description: 创建server_conhash_t实例.
 *   Parameter: lua_file lua文件
 *      Return: 返回server_conhash_t实例
 *              NULL -> 创建失败.
 */
server_conhash_t *server_conhash_create(char *lua_file);

/** 
 *        Name: server_conhash_destroy
 * Description: 销毁实例.
 *   Parameter: server_hash.
 *      Return: 
 */
void server_conhash_destroy(server_conhash_t **server_conhash);

/** 
 *        Name: server_conhash_find
 * Description: 根据IDC ID和url得到相应的Server的IP.
 *   Parameter: group -> IDC ID.
 *              url -> 视频URL.
 *      Return: 
 */
char *server_conhash_find(server_conhash_t *server_conhash, int group_id, char *url);

#endif
