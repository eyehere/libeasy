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
#ifndef _ENETWORK_H_
#define _ENETWORK_H_
#include "ecommon.h"
#include "elog.h"

/** 
 *        Name: udp_server_create
 * Description: 创建udp socket的服务器端，并设置为异步。 
 *   Parameter: port -> 端口。
 *      Return: -1 -> 创建失败。
 *              >=0 返回创建的upd socket的句柄。
 */
int udp_server_create(int port);

/** 
 *        Name: udp_client_create
 * Description: 创建udp socket ，并设置为异步。 
 *   Parameter: 
 *      Return: -1 -> 创建失败。
 *              >=0 返回创建的upd socket的句柄。
 */
int udp_client_create();

/** 
 *        Name: unix_udp_server_create
 * Description: 创建unix domain udp socket的服务器端，并设置为异步。 
 *   Parameter: sock_path -> unix socket路径。
 *      Return: -1 -> 创建失败。
 *              >=0 返回创建的unix udp socket的句柄。
 */
int unix_udp_server_create(char *sock_path);

/** 
 *        Name: unix_udp_client_create
 * Description: 创建unix domain udp socket的客户端，并设置为异步。 
 *   Parameter: 
 *      Return: -1 -> 创建失败。
 *              >=0 返回创建的unix udp socket的句柄。
 */
int unix_udp_client_create();

/** 
 *        Name: sock_set_nonblocking
 * Description: 。 
 *   Parameter: sock_fd -> socket描述符 
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功。
 */
int sock_set_nonblocking(int sock_fd);

/** 
 *        Name: sock_set_rcv_buffer_size
 * Description: 设置接收缓冲区大小 
 *   Parameter: sockd_fd -> socket描述符
 *              size -> 接收缓冲区大小
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功 。
 */
int sock_set_rcv_buffer_size(int sock_fd, int size);

/** 
 *        Name: sock_set_snd_buffer_size
 * Description: 设置发送缓冲区大小 
 *   Parameter: sockd_fd -> socket描述符
 *              size -> 发送缓冲区大小
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功 。
 */
int sock_set_snd_buffer_size(int sock_fd, int size);

/** 
 *        Name: is_valid_ip
 * Description: 是否是合法IP 
 *   Parameter: ip_str -> 是否是合法IP 
 *      Return: 0 -> 不是合法IP。
 *              1 -> 合法IP。
 */
int is_valid_ip(const char *ip_str);
#endif
