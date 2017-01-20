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
#ifndef UDP_H_
#define UDP_H_
#include "easy.h"
#include "network_util.h"

/** 
 *        Name: udp_unix_socket_create
 * Description: 创建unix domain udp socket
 *   Parameter: unix_path -> 本地unix path
 *      Return: 返回unix domain socket文件描述符
 *              -1 -> 失败 
 */
int udp_unix_socket_create(char *unix_path);

/** 
 *        Name: udp_unix_socket_send
 * Description: unix domain udp socket send data
 *   Parameter: fd -> sock描述符
 *              peer_path -> 对等节点地址
 *              buffer -> 发送数据buffer
 *              size -> 发送数据字节数
 *      Return: >= 0 -> 发送成功
 *              -1   -> 发送失败
 */
int udp_unix_socket_send(int fd, char *peer_path, char *buffer, int size);

/** 
 *        Name: udp_unix_socket_recv
 * Description: unix domain udp socket recv data
 *   Parameter: fd -> sock描述符
 *              buffer -> 接收数据的buffer
 *              size -> 接收数据buffer的大小
 *      Return: 接收的字节数
 *              -1 -> 接收错误
 *              
 */
int udp_unix_socket_recv(int fd, char *buffer, int size);

/** 
 *        Name: udp_unix_socket_close 
 * Description: 关闭unix domain udp socket
 *   Parameter: fd -> sock描述符
 *      Return:  
 *              
 */
void udp_unix_socket_close(int fd);

#endif
