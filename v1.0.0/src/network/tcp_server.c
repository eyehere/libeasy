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
#include "tcp_server.h"

static tcp_server_stream_t *get_idle_tcp_stream(tcp_server_t *server);
static void tcp_stream_close(tcp_server_stream_t *stream);
static void tcp_stream_init(tcp_server_t *server, tcp_server_stream_t *stream, 
        int sock_fd, struct sockaddr_in *remote_addr);

static int accept_cb(tcp_server_stream_t *stream);
static int rcv_cb_default(tcp_server_stream_t *stream);
static int snd_cb_default(tcp_server_stream_t *stream);

static void delete_stream_from_busy_list(tcp_server_t *server, tcp_server_stream_t *stream);
static void add_stream_into_busy_list(tcp_server_t *server, tcp_server_stream_t *stream);
static void delete_stream_from_idle_list(tcp_server_t *server, tcp_server_stream_t *stream);
static void add_stream_into_idle_list(tcp_server_t *server, tcp_server_stream_t *stream);

tcp_server_t *tcp_server_create(int port, int max_conn)
{
    tcp_server_t *server = NULL;
    tcp_server_stream_t *stream = NULL;
    int max_conn_count = TCP_MAX_STREAM_NUM;
    int i = 0;
    struct sockaddr_in server_addr;
    struct epoll_event event;

    server = (tcp_server_t *)malloc(sizeof(tcp_server_t));
    if (NULL == server) {
        goto error; 
    }
    bzero(server, sizeof(tcp_server_t));
    server->rcv_cb = rcv_cb_default;
    server->snd_cb = snd_cb_default;
    
    server->port = port;

    if (max_conn > 0) {
        max_conn_count = max_conn; 
    }

    /* 创建并发连接结构 */
    server->streams = (tcp_server_stream_t *)malloc(sizeof(tcp_server_stream_t) * max_conn_count);
    if (NULL == server->streams) {
        log_error("malloc for server->streams failed.");
        goto error;
    }
    bzero(server->streams, sizeof(tcp_server_stream_t) * max_conn_count);
    server->stream_num = max_conn_count;

    /* idle stream链表初始化 */
    for (i = 0; i < max_conn_count; i++) {
        server->streams[i].index = i;
        add_stream_into_idle_list(server, &server->streams[i]); 
    }

    /* 创建listen socket */
    server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server->listen_fd) {
        log_error("create socket failed, %s", strerror(errno));
        goto error;
    }

    /* 设置为非阻塞 */
    if (!set_non_blocking(server->listen_fd)){
        log_error("set_non_blocking failed.");
        goto error;
    }

    /* 设置地址重用 */
    if (!set_reuse_addr(server->listen_fd)) {
        log_error("set reuse addr failed.");
        goto error;
    }

    /* 绑定端口 */
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(server->port);
    if (bind(server->listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_error("bind server addr failed, %s", strerror(errno));
        goto error;
    }

    /* 设置监听 */
    if (listen(server->listen_fd, TCP_MAX_LISTEN_NUM) < 0) {
        log_error("listen failed, %s", strerror(errno));
        goto error;
    }

    /* 创建epoll句柄 */
    server->epoll_fd = epoll_create(TCP_MAX_CONNECT_NUM);
    if (server->epoll_fd < 0) {
        log_error("epoll_create failed, %s", strerror(errno));
        goto error;
    }

    /* 获取空闲的stream */
    stream = get_idle_tcp_stream(server);
    if (NULL == stream) {
        log_error("get_idle_tcp_stream for listen stream failed.");
        goto error;
    }

    /* 将listen fd加入epoll */
    bzero(&event, sizeof(event));
    event.data.fd  = server->listen_fd;
    event.data.ptr = stream;
    event.events   = EPOLLIN | EPOLLET;

    /* 创建listen stream */
    tcp_stream_init(server, stream, server->listen_fd, NULL);
    stream->snd_cb = NULL; 
    stream->rcv_cb = accept_cb;

    add_stream_into_busy_list(server, stream);

    return server;

error:
    tcp_server_destroy(server); 

    return NULL;
}

int tcp_server_run(tcp_server_t *server)
{
    struct epoll_event *events = NULL;
    tcp_server_stream_t       *stream = NULL;
    int nfds = 0;
    int i    = 0;

    if (NULL == server) {
        log_error("server is NULL.");
        return 0;
    }

    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * server->stream_num);
    if (NULL == events) {
        log_error("malloc for events failed.");
        return 0;
    }

    while (!server->stop) {
        nfds = epoll_wait(server->epoll_fd, events, server->stream_num, TCP_EPOLL_TIMEOUT);
        if (nfds < 0) {
            log_error("epoll_wait error, %s.", strerror(errno));
            return 0;
        }

        for (i = 0; i < nfds; i++) {
            stream = (tcp_server_stream_t *)events[i].data.ptr;
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
                log_error("epoll event has error, remote ip: %s", inet_ntoa(stream->remote_addr.sin_addr));
                stream->error = 1; 
            }
            else if (events[i].events & EPOLLHUP) { 
                log_warn("epoll event has EPOLLHUP, remote ip: %s", inet_ntoa(stream->remote_addr.sin_addr));
            }
            else {
                log_error("epoll event unknown flags, %s.", inet_ntoa(stream->remote_addr.sin_addr));
            }

            if (stream->error || stream->close) {
                tcp_stream_close(stream); 
                server->active_stream_num--;
            }
        }
    }

    return 1;
}

void tcp_server_stop(tcp_server_t *server)
{
    server->stop = 1; 
}

void tcp_server_set_rcv_cb(tcp_server_t *server, tcp_server_rcv_cb_t cb)
{
    server->rcv_cb = cb;
}

void tcp_server_set_snd_cb(tcp_server_t *server, tcp_server_snd_cb_t cb)
{
    server->snd_cb = cb;
}

int tcp_server_read(tcp_server_stream_t *stream, buffer_t *buffer)
{
    return 1;
}

void tcp_server_destroy(tcp_server_t *server)
{
    if (NULL == server) {
        return;
    }

    if (server->streams) {
        free(server->streams);
    }

    if (server->listen_fd >= 0) {
        close(server->listen_fd);
    }

    if (server->epoll_fd >= 0) {
        close(server->epoll_fd);
    }

    free(server);
}

/*
 * 得到一个空闲的stream资源
 */
static tcp_server_stream_t *get_idle_tcp_stream(tcp_server_t *server)
{
    if (server->idle_stream_tail) {
        tcp_server_stream_t *stream; 

        stream = server->idle_stream_tail;
        
        delete_stream_from_idle_list(server, stream);
          
        return stream;
    }

    return NULL;
}

static void tcp_stream_close(tcp_server_stream_t *stream)
{
    tcp_server_t *server = NULL;
    if (NULL == stream) {
        return; 
    }

    server = stream->server;

    if (stream->pool) {
        mem_pool_destroy(&stream->pool);
    }

    if (stream->sock_fd >= 0) {
        close(stream->sock_fd);
    }

    /* 从busy链表中删除 */
    delete_stream_from_busy_list(server, stream); 
     
    /* 放入到idle链表尾部 */
    add_stream_into_idle_list(server, stream);  

    /* reset stream */
    stream->sock_fd = -1;
    stream->pool    = NULL;
    stream->close   = 0;
    stream->error   = 0;
    stream->data    = NULL;
    stream->server  = NULL;
}

static void tcp_stream_init(tcp_server_t *server, tcp_server_stream_t *stream, 
        int sock_fd, struct sockaddr_in *remote_addr)
{
    if (NULL == stream) {
        return;
    }

    stream->sock_fd = sock_fd;
    stream->pool = mem_pool_create();
    if (NULL == stream->pool) {
        log_error("create tcp stream pool failed."); 
        return;
    }
    
    stream->close  = 0;
    stream->error  = 0;

    gettimeofday(&stream->update_time, NULL);

    stream->data   = NULL;
    stream->server = server;

    if (NULL == server->rcv_cb) {
        stream->rcv_cb = rcv_cb_default;
    }
    else {
        stream->rcv_cb = server->rcv_cb;
    }

    if (NULL == server->snd_cb) {
        stream->snd_cb = snd_cb_default;
    }
    else {
        stream->snd_cb = server->snd_cb;
    }

    if (remote_addr) {
        memcpy(&stream->remote_addr, remote_addr, sizeof(struct sockaddr_in));
    }
}

static int accept_cb(tcp_server_stream_t *listen_stream)
{
    struct sockaddr_in remote_addr;
    struct epoll_event event;
    socklen_t          addr_len;
    int                sock_fd;
    tcp_server_t      *server;
    tcp_server_stream_t      *stream;

    bzero(&remote_addr, sizeof(remote_addr));
    addr_len = sizeof(struct sockaddr_in); 

    sock_fd = accept(listen_stream->sock_fd, (struct sockaddr *)&remote_addr, &addr_len);
    if (sock_fd < 0) {
        if (EAGAIN != errno) {
            log_error("accept error, %s.", strerror(errno)); 
            return 0;
        }
        else {
            log_info("no connection for accept."); 
            return 1;
        }
    }

    log_debug("new connection comes, fd: %d.", sock_fd);
    if (!set_non_blocking(sock_fd)) {
        log_error("set sock_fd: %d failed.", sock_fd);
        close(sock_fd);
        return 0;
    }

    stream = get_idle_tcp_stream(server);
    if (NULL == stream) {
        log_error("no idle stream.");
        return 0;
    }

    tcp_stream_init(server, stream, sock_fd, &remote_addr); 
    add_stream_into_busy_list(server, stream);

    /* 将当前sock fd添加到epoll中 */
    bzero(&event, sizeof(event));
    event.data.fd  = sock_fd;
    event.data.ptr = stream;
    event.events   = EPOLLIN | EPOLLET;

    if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
        log_error("accept_cb, EPOLL_CTL_ADD error, %s.", strerror(errno)); 
        tcp_stream_close(stream);
        return 0;
    }
    server->active_stream_num++;

    return 1;
}

static int rcv_cb_default(tcp_server_stream_t *stream)
{
    printf("rcv_cb_default\n");
    return 1; 
}

static int snd_cb_default(tcp_server_stream_t *stream)
{
    printf("snd_cb_default\n");
    return 1; 
}

/*
 * 从busy双向链表中删除stream
 */
static void delete_stream_from_busy_list(tcp_server_t *server, tcp_server_stream_t *stream)
{
    /* 如果当前节点的上一个节点存在 */
    if (stream->last) {
        stream->last->next = stream->next; 
    }
    /* 如果上一个节点不存在，需要修改头部指针 */
    else {
        server->busy_stream_head = stream->next; 
    }
    
    /* 如果下一个节点存在 */
    if (stream->next) {
        stream->next->last = stream->last; 
    }
    /* 如果下一个节点不存在，需要修改尾部指针 */
    else {
        server->busy_stream_tail = stream->last;
    }
    server->busy_stream_num--;
}

/*
 * 将stream添加到busy双向链表中
 */
static void add_stream_into_busy_list(tcp_server_t *server, tcp_server_stream_t *stream)
{
    if (NULL == server->busy_stream_head || NULL == server->busy_stream_tail) {
        server->busy_stream_head = stream;
        server->busy_stream_tail = stream;
        stream->last = NULL;
        stream->next = NULL;
        server->busy_stream_num++;
        return;
    }
    
    server->busy_stream_tail->next = stream;
    stream->next                   = NULL;
    stream->last                   = server->busy_stream_tail;
    server->busy_stream_tail       = stream;
    server->busy_stream_num++;
}

/* 
 *从idle双向链表中删除stream
 */
static void delete_stream_from_idle_list(tcp_server_t *server, tcp_server_stream_t *stream)
{
    /* 如果当前节点的上一个节点存在 */
    if (stream->last) {
        stream->last->next = stream->next; 
    }
    /* 如果上一个节点不存在，需要修改头部指针 */
    else {
        server->idle_stream_head = stream->next; 
    }
    
    /* 如果下一个节点存在 */
    if (stream->next) {
        stream->next->last = stream->last; 
    }
    /* 如果下一个节点不存在，需要修改尾部指针 */
    else {
        server->idle_stream_tail = stream->last;
    }
    server->idle_stream_num--;
}

/* 
 * 将stream添加到idle双向链表中
 */
static void add_stream_into_idle_list(tcp_server_t *server, tcp_server_stream_t *stream)
{
    if (NULL == server->idle_stream_head || NULL == server->idle_stream_tail) {
        server->idle_stream_head = stream;
        server->idle_stream_tail = stream;
        stream->last = NULL;
        stream->next = NULL;
        server->idle_stream_num++;
        return;
    }
    
    server->idle_stream_tail->next = stream;
    stream->next                   = NULL;
    stream->last                   = server->idle_stream_tail;
    server->idle_stream_tail       = stream;
    server->idle_stream_num++;
}
