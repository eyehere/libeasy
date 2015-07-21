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
#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_
#include "easy.h"
#include "network_util.h"

#define TCP_CLIENT_OK     0
#define TCP_CLIENT_ERROR -1

#define TCP_MAX_LISTEN_NUM      256
#define TCP_EPOLL_TIMEOUT       50
#define TCP_MAX_IDLE_TIME       10000
#define TCP_MAX_STREAM_NUM      10000
#define TCP_MAX_CONNECT_NUM     2048

#define TCP_STATE_NONE         0
#define TCP_STATE_CONNECTING   1
#define TCP_STATE_CONNECTED    2

typedef struct _tcp_client_stream_t tcp_client_stream_t;
typedef struct _tcp_client_t tcp_client_t;

typedef int (*tcp_client_rcv_cb_t)(tcp_client_stream_t *stream);
typedef int (*tcp_client_snd_cb_t)(tcp_client_stream_t *stream);
typedef int (*tcp_client_conn_cb_t)(tcp_client_stream_t *stream);

struct _tcp_client_stream_t {
    int                  index;               /* 每一个stream在stream数组中的下标,一旦初始化便不会改变 */
    int                  sock_fd;             /* socket文件描述符 */
    char                *server_ip;           /* server IP地址 */
    int                  port;                /* 服务器端口 */
    struct sockaddr_in   server_addr;         /* 服务器端地址 */
    mem_pool_t          *pool;                /* 每一个连接请求负责内存分配的内存池 */
    int                  close;               /* 是否要关闭连接 */
    int                  error;               /* 连接是否有错误 */
    struct timeval       update_time;         /* stream上一次活跃的时间 */
    buffer_chain_t      *chain;               /* 需要发送的数据链 */
    buffer_chain_t      *cur_chain;           /* 需要发送数据链当前发送节点 */
    int                  tcp_state;           /* TCP状态，连接中；连接成功 */

    void                *data;                /* 每一个连接的私有数据 */
    tcp_client_t        *client;              /* 指向tcp client */

    tcp_client_stream_t *last;                /* 在busy或者idle stream双向链表中的上一个stream */
    tcp_client_stream_t *next;                /* 在busy或者idle stream双向链表中的下一个stream */

    tcp_client_rcv_cb_t  rcv_cb;              /* 接收数据回调函数 */
    tcp_client_snd_cb_t  snd_cb;              /* 发送数据回调函数 */
    tcp_client_conn_cb_t conn_cb;             /* 连接回调函数 */
};

struct _tcp_client_t {
    int           epoll_fd;                   /* epoll文件描述符 */
    int           max_idle_time;              /* 连接的最大超时时间 */

    tcp_client_stream_t *streams;             /* 支持的stream的数组 */
    int                  stream_num;          /* 支持的最大的stream的数量 */
    int           active_stream_num;          /* 当前活跃的stream数量 */

    tcp_client_stream_t *busy_stream_head;    /* busy stream链表头 */
    tcp_client_stream_t *busy_stream_tail;    /* busy stream链表尾 */
    int                  busy_stream_num;     /* busy stream的数量 */

    tcp_client_stream_t *idle_stream_head;    /* idle stream链表头 */
    tcp_client_stream_t *idle_stream_tail;    /* idle stream链表尾 */
    int                  idle_stream_num;     /* idle stream的数量 */

    int                  stop;
};

/** 
 *        Name: tcp_client_create
 * Description: 创建tcp client
 *   Parameter: max_conn -> 最大支持的
 *              如果max_conn小于等于0，则设置成默认的最大并发连接数
 *      Return: 返回tcp client实例 
 *              NULL -> 创建失败
 */
tcp_client_t *tcp_client_create(int max_conn);

/** 
 *        Name: tcp_client_run
 * Description: 启动tcp client
 *   Parameter: client -> tcp client实例
 *      Return: 1 -> 启动成功
 *              0 -> 启动失败
 */
int tcp_client_run(tcp_client_t *client);

/** 
 *        Name: tcp_client_stop
 * Description: 停止tcp client
 *   Parameter: client -> tcp client实例
 *      Return: 1 -> 启动成功
 *              0 -> 启动失败
 */
void tcp_client_stop(tcp_client_t *client);

/** 
 *        Name: tcp_client_add_server
 * Description: 向tcp client实例添加server
 *   Parameter: server_ip   -> 服务器IP
 *              server_port -> 服务器端口
 *      Return: 1 -> 添加成功
 *              0 -> 添加失败
 */
int tcp_client_add_server(tcp_client_t *client, 
                          char *server_ip, int server_port, 
                          tcp_client_rcv_cb_t rcv_cb, 
                          tcp_client_snd_cb_t snd_cb, 
                          tcp_client_conn_cb_t conn_cb);

/** 
 *        Name: tcp_client_read
 * Description: 从tcp流读取buffer描述的字节数
 *              
 *   Parameter: stream -> 描述当前连接的实例
 *              buffer -> 读取数据的buffer
 *              buffer->data的空间应该在函数外部分配，
 *              buffer->len应该是分陪内存的字节数。
 *              函数返回后，buffer->len返回真实读取的字节数
 *      Return: 1 -> 读取成功
 *              0 -> 读取失败
 */
int tcp_client_read(tcp_client_stream_t *stream, buffer_t *buffer);

/** 
 *        Name: tcp_client_send
 * Description: 发送数据
 *              
 *   Parameter: stream -> 描述当前连接的实例
 *              data   -> 发送数据的buffer
 *              size   -> 发送数据大小，以字节为单位
 *      Return: 1 -> 发送成功
 *              0 -> 发送失败
 */
int tcp_client_send(tcp_client_stream_t *stream, void *data, int size);

/** 
 *        Name: tcp_client_destroy
 * Description: 销毁tcp client实例
 *   Parameter: tcp_client -> tcp client实例
 *      Return:  
 */
void tcp_client_destroy(tcp_client_t *tcp_client);

#endif
