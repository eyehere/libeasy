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
#include "enetwork.h"

int udp_server_create(int port)
{
    struct sockaddr_in server_addr;
    int udp_sock_fd = -1;

    udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == udp_sock_fd){
        log_error("create udp socket failed, %s.", strerror(errno));
        return -1;
    }

    if(!sock_set_nonblocking(udp_sock_fd)){
        close(udp_sock_fd);
        return -1;
    }

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port);

    if(bind(udp_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0){
        log_error("bind udp socket failed, %s.", strerror(errno));
        close(udp_sock_fd);
        return -1;
    }

    return udp_sock_fd;
}

int udp_client_create()
{
    int udp_sock_fd = -1;

    udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == udp_sock_fd){
        log_error("create udp socket failed, %s.", strerror(errno));
        return -1;
    }

    if(!sock_set_nonblocking(udp_sock_fd)){
        close(udp_sock_fd);
        return -1;
    }

    return udp_sock_fd;
}

int unix_udp_server_create(char *sock_path)
{
    int unix_udp_fd = -1;
    struct sockaddr_un addr;

    unix_udp_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(-1 == unix_udp_fd){
        log_error("create unix udp socket failed, %s.", strerror(errno));
        return -1;
    }

    if(!sock_set_nonblocking(unix_udp_fd)){
        close(unix_udp_fd);
        return -1;
    }

    bzero(&addr, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    unlink(sock_path);

    if(bind(unix_udp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        log_error("bind unix udp socket failed, %s.", strerror(errno));
        close(unix_udp_fd);
        return -1;
    }

    return unix_udp_fd;
}

int unix_udp_client_create()
{
    int unix_udp_fd = -1;

    unix_udp_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(-1 == unix_udp_fd){
        log_error("create unix udp socket failed, %s.", strerror(errno));
        return -1;
    }

    if(!sock_set_nonblocking(unix_udp_fd)){
        close(unix_udp_fd);
        return -1;
    }

    return unix_udp_fd;
}

int sock_set_nonblocking(int sock_fd)
{
    int opts;
    opts = fcntl(sock_fd, F_GETFL);
    if(opts < 0){
        log_error("fcntl error, %s.", strerror(errno));
        return 0;
    }

    opts = opts | O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, opts)<0){
        log_error("fcntl error, %s.", strerror(errno));
        return 0;
    }

    return 1;
}

int sock_set_rcv_buffer_size(int sock_fd, int size)
{
    int rcv_size = 0; 
    socklen_t opt_len  = 0;

    opt_len = sizeof(rcv_size);
    if(getsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &opt_len) < 0){
        log_error("getsockopt failed, %s.", strerror(errno));
        return 0;
    }

    rcv_size = size;
    opt_len  = sizeof(rcv_size);
    if(setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, opt_len) < 0){
        log_error("setsockopt failed, %s.", strerror(errno));
        return 0;
    }
    return 1;
}

int sock_set_snd_buffer_size(int sock_fd, int size)
{
    int snd_size = 0; 
    socklen_t opt_len  = 0;

    opt_len = sizeof(snd_size);
    if(getsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, &opt_len) < 0){
        log_error("getsockopt failed, %s.", strerror(errno));
        return 0;
    }

    snd_size = size;
    opt_len  = sizeof(snd_size);
    if(setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, opt_len) < 0){
        log_error("setsockopt failed, %s.", strerror(errno));
        return 0;
    }
    return 1;
}

int is_valid_ip(const char *ip_str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));

    if (result == 0) 
    { 
        return result; 
    } 
    return 1;
}
