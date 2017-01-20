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
#include "tcp_client.h"

static tcp_client_stream_t *get_idle_tcp_client_stream(tcp_client_t *client);
static void tcp_client_stream_reset(tcp_client_stream_t *stream);
static int tcp_client_stream_init(tcp_client_t *client, 
                                  tcp_client_stream_t *stream, 
                                  char *server_ip,
                                  int port,
                                  tcp_client_rcv_cb_t rcv_cb,
                                  tcp_client_snd_cb_t snd_cb,
                                  tcp_client_conn_cb_t conn_cb);

static int rcv_cb_default(tcp_client_stream_t *stream);
static int snd_cb_default(tcp_client_stream_t *stream);

static void delete_stream_from_busy_list(tcp_client_t *client, tcp_client_stream_t *stream);
static void add_stream_into_busy_list(tcp_client_t *client, tcp_client_stream_t *stream);
static void delete_stream_from_idle_list(tcp_client_t *client, tcp_client_stream_t *stream);
static void add_stream_into_idle_list(tcp_client_t *client, tcp_client_stream_t *stream);

tcp_client_t *tcp_client_create(int max_conn)
{
    tcp_client_t *client = NULL;
    int   max_conn_count = TCP_MAX_STREAM_NUM;
    int                i = 0;

    client = (tcp_client_t *)malloc(sizeof(tcp_client_t));
    if (NULL == client) {
        goto error; 
    }
    bzero(client, sizeof(tcp_client_t));
    client->rcv_cb = rcv_cb_default;
    client->snd_cb = snd_cb_default;

    if (max_conn > 0) {
        max_conn_count = max_conn; 
    }

    /* 创建并发连接结构 */
    client->streams = (tcp_client_stream_t *)malloc(sizeof(tcp_client_stream_t) * max_conn_count);
    if (NULL == client->streams) {
        log_error("malloc for client->streams failed.");
        goto error;
    }
    bzero(client->streams, sizeof(tcp_client_stream_t) * max_conn_count);
    client->stream_num = max_conn_count;

    /* idle stream链表初始化 */
    for (i = 0; i < max_conn_count; i++) {
        client->streams[i].index   = i;
        client->streams[i].sock_fd = -1;
        add_stream_into_idle_list(client, &client->streams[i]); 
    }

    /* 创建epoll句柄 */
    client->epoll_fd = epoll_create(TCP_MAX_CONNECT_NUM);
    if (client->epoll_fd < 0) {
        log_error("epoll_create failed, %s", strerror(errno));
        goto error;
    }
    
    return client;

error:
    tcp_client_destroy(client); 

    return NULL;
}

int tcp_client_run(tcp_client_t *client)
{
    struct epoll_event  *events = NULL;
    tcp_client_stream_t *stream = NULL;
    int nfds = 0;
    int i    = 0;

    if (NULL == client) {
        log_error("client is NULL.");
        return 0;
    }

    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * client->stream_num);
    if (NULL == events) {
        log_error("malloc for events failed.");
        return 0;
    }

    while (!client->stop) {
        nfds = epoll_wait(client->epoll_fd, events, client->stream_num, TCP_EPOLL_TIMEOUT);
        if (nfds < 0) {
            log_error("epoll_wait error, %s.", strerror(errno));
            return 0;
        }

        for (i = 0; i < nfds; i++) {
            stream = (tcp_client_stream_t *)events[i].data.ptr;
            if (NULL == stream) {
                log_error("events[%d].data.ptr is NULL", i);
                continue;
            }

            if (events[i].events & EPOLLIN && stream->rcv_cb) {
                stream->rcv_cb(stream); 
            }
            else if (events[i].events & EPOLLOUT && stream->snd_cb) {
                stream->snd_cb(stream);
            }
            else if ((events[i].events & EPOLLERR)) { 
                stream->error = 1; 
            }
            else if (events[i].events & EPOLLHUP) { 
            }
            else {
            }

            if (stream->error || stream->close) {
                tcp_stream_close(stream); 
                client->active_stream_num--;
            }
        }
    }

    return 1;
}

void tcp_client_stop(tcp_client_t *client)
{
    client->stop = 1; 
}

int tcp_client_add_server(tcp_client_t *client, 
                          char *server, int server_port, 
                          tcp_client_rcv_cb_t rcv_cb, 
                          tcp_client_snd_cb_t snd_cb, 
                          tcp_client_conn_cb_t conn_cb)
{
    tcp_client_stream_t *stream  = NULL;
   
    stream = get_idle_tcp_client_stream(client);
    if (NULL == stream) {
        log_error("get idle tpc client stream failed for %s.", server);
        return 0;
    }

    if (!tcp_client_stream_init(client, stream, server_ip, port,
                           rcv_cb, snd_cb, conn_cb)) {
        log_error("tcp_client_stream_init failed.");
        add_stream_into_idle_list(client, stream);
        return 0;
    }
    
    add_stream_into_busy_list(client, stream);
    return 1;
}

int tcp_client_read(tcp_client_stream_t *stream, buffer_t *buffer)
{
    return 1;
}

int tcp_client_send(tcp_client_stream_t *stream, void *data, int size)
{
    return 1;
}

void tcp_client_destroy(tcp_client_t *client)
{
    if (NULL == client) {
        return;
    }

    if (client->streams) {
        free(client->streams);
    }

    if (client->epoll_fd >= 0) {
        close(client->epoll_fd);
    }

    free(client);
}

/*
 * 得到一个空闲的stream资源
 */
static tcp_client_stream_t *get_idle_tcp_client_stream(tcp_client_t *client)
{
    if (client->idle_stream_tail) {
        tcp_client_stream_t *stream; 

        stream = client->idle_stream_tail;
        
        delete_stream_from_idle_list(client, stream);
          
        return stream;
    }

    return NULL;
}

static void tcp_client_stream_reset(tcp_client_stream_t *stream)
{
    tcp_client_t *client = NULL;
    if (NULL == stream) {
        return; 
    }

    client = stream->client;

    if (stream->pool) {
        mem_pool_destroy(&stream->pool);
    }

    if (stream->sock_fd >= 0) {
        close(stream->sock_fd);
    }

    /* reset stream */
    stream->sock_fd   = -1;
    stream->server_ip = NULL;
    stream->port      = 0;
    stream->pool      = NULL;
    stream->close     = 0;
    stream->error     = 0;
    stream->chain     = NULL;
    stream->cur_chain = NULL;
    stream->tcp_state = TCP_STATE_NONE;
    stream->data      = NULL;
    stream->client    = NULL;
    stream->last      = NULL;
    stream->next      = NULL;
    stream->rcv_cb    = NULL;
    stream->snd_cb    = NULL;
    stream->conn_cb   = NULL;

    bzero(&stream->update_time, NULL);
    bzero(&stream->dst_addr, NULL);
}

static int tcp_client_stream_init(tcp_client_t *client, 
                                  tcp_client_stream_t *stream, 
                                  char *server_ip,
                                  int port,
                                  tcp_client_rcv_cb_t rcv_cb,
                                  tcp_client_snd_cb_t snd_cb,
                                  tcp_client_conn_cb_t conn_cb)
{
    struct hostent      *hostp   = NULL;
    struct sockaddr_in   server_addr;
    struct epoll_event   event;
    int                  connected = 0;  

    if (NULL == stream) {
        return;
    }

    stream->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == stream->sock_fd) {
        log_error("create socket for %s failed, %s.", server, strerror(errno));
        goto error;
    }

    if (!set_non_blocking(stream->sock_fd)) {
        log_error("set non blocking failed, %s:%d.", server_ip, port);
        goto error;
    }

    stream->pool = mem_pool_create();
    if (NULL == stream->pool) {
        log_error("create tcp stream pool failed."); 
        goto error;
    }
    
    stream->close  = 0;
    stream->error  = 0;

    gettimeofday(&stream->update_time, NULL);

    stream->data   = NULL;
    stream->client = client;

    stream->server_ip = (char *)mem_pool_calloc(stream->pool, strlen(server_ip) + 1);
    if (NULL == stream->server_ip) {
        log_error("mem_pool_calloc for server_ip failed.");
        goto error;
    }
    strcpy(stream->server_ip, server_ip);
    stream->port      = port;
    stream->tcp_state = TCP_STATE_CONNECTING;

    stream->server_addr.sin_family      = AF_INET;
    stream->server_addr.sin_port        = htons(port);
    stream->server_addr.sin_addr.s_addr = inet_addr(server_ip); 

    if ((unsigned long)INADDR_NONE == stream->server_addr.sin_addr.s_addr) {
        hostp = gethostbyname(server_ip); 
        if ((struct hostent *)NULL == hostp) {
            log_error("gethostbyname for %s failed.", server);
            goto error; 
        }
        memcpy(&stream->server_addr.sin_addr, hostp->h_addr, sizeof(struct sockaddr_in));
    }

    connected = connect(stream->sock_fd, &stream->server_addr, sizeof(struct sockaddr_in));
    /* 连接失败 */
    if (-1 == connected && errno != EINPROGRESS) {
        log_error("connect %s:%d failed.", stream->server_ip, stream->port); 
        goto error;
    }
    
    bzero(&event, sizeof(event));
    event.data.fd  = sock_fd;
    event.data.ptr = stream;
    event.events   = EPOLLOUT | EPOLLERR;

    /* 连接完成 */
    if (connected == 0) {
        stream->tcp_state = TCP_STATE_CONNECTED;
    }

    if (epoll_ctl(client->epoll_fd, EPOLL_CTL_ADD, stream->sock_fd, &event) < 0) {
        log_error("EPOLL_CTL_ADD failed.");
        goto error;
    }

    stream->rcv_cb  = rcv_cb;
    stream->snd_cb  = snd_cb;
    stream->conn_cb = conn_cb;

    return 1;

error:
    tcp_client_stream_reset(stream);
    return 0;
}

static int rcv_cb_default(tcp_client_stream_t *stream)
{
    printf("rcv_cb_default\n");
    return 1; 
}

static int snd_cb_default(tcp_client_stream_t *stream)
{
    printf("snd_cb_default\n");
    return 1; 
}

/*
 * 从busy双向链表中删除stream
 */
static void delete_stream_from_busy_list(tcp_client_t *client, tcp_client_stream_t *stream)
{
    /* 如果当前节点的上一个节点存在 */
    if (stream->last) {
        stream->last->next = stream->next; 
    }
    /* 如果上一个节点不存在，需要修改头部指针 */
    else {
        client->busy_stream_head = stream->next; 
    }
    
    /* 如果下一个节点存在 */
    if (stream->next) {
        stream->next->last = stream->last; 
    }
    /* 如果下一个节点不存在，需要修改尾部指针 */
    else {
        client->busy_stream_tail = stream->last;
    }
    client->busy_stream_num--;
}

/*
 * 将stream添加到busy双向链表中
 */
static void add_stream_into_busy_list(tcp_client_t *client, tcp_client_stream_t *stream)
{
    if (NULL == client->busy_stream_head || NULL == client->busy_stream_tail) {
        client->busy_stream_head = stream;
        client->busy_stream_tail = stream;
        stream->last = NULL;
        stream->next = NULL;
        client->busy_stream_num++;
        return;
    }
    
    client->busy_stream_tail->next = stream;
    stream->next                   = NULL;
    stream->last                   = client->busy_stream_tail;
    client->busy_stream_tail       = stream;
    client->busy_stream_num++;
}

/* 
 *从idle双向链表中删除stream
 */
static void delete_stream_from_idle_list(tcp_client_t *client, tcp_client_stream_t *stream)
{
    /* 如果当前节点的上一个节点存在 */
    if (stream->last) {
        stream->last->next = stream->next; 
    }
    /* 如果上一个节点不存在，需要修改头部指针 */
    else {
        client->idle_stream_head = stream->next; 
    }
    
    /* 如果下一个节点存在 */
    if (stream->next) {
        stream->next->last = stream->last; 
    }
    /* 如果下一个节点不存在，需要修改尾部指针 */
    else {
        client->idle_stream_tail = stream->last;
    }

    stream->last = NULL;
    stream->next = NULL;
    client->idle_stream_num--;
}

/* 
 * 将stream添加到idle双向链表中
 */
static void add_stream_into_idle_list(tcp_client_t *client, tcp_client_stream_t *stream)
{
    if (NULL == client->idle_stream_head || NULL == client->idle_stream_tail) {
        client->idle_stream_head = stream;
        client->idle_stream_tail = stream;
        stream->last = NULL;
        stream->next = NULL;
        client->idle_stream_num++;
        return;
    }
    
    client->idle_stream_tail->next = stream;
    stream->next                   = NULL;
    stream->last                   = client->idle_stream_tail;
    client->idle_stream_tail       = stream;
    client->idle_stream_num++;
}
