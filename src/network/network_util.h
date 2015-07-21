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
#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_
#include "easy.h"
#include "ecommon.h"

/** 
 *        Name: set_non_blocking
 * Description: 设置非阻塞 
 *   Parameter: sock_fd -> sock描述符
 *      Return: 0 -> 设置失败
 *              1 -> 设置成功 
 */
int set_non_blocking(int sock_fd);

/** 
 *        Name: set_reuse_addr
 * Description: 设置地址重用
 *   Parameter: sock_fd -> sock描述符
 *      Return: 0 -> 设置失败
 *              1 -> 设置成功 
 */
int set_reuse_addr(int sock_fd);

/** 
 *        Name: nic_get_ip
 * Description: 根据网卡名称获取网卡IP
 *   Parameter: name -> 网卡名称，例如eth0
 *              ip   -> 输出IP
 *              ip_size -> 输出ip的buffer大小
 *      Return: 0 -> 获取失败
 *              1 -> 获取成功 
 */
int nic_get_ip(char *name, char *ip, int ip_size);
    
#endif
