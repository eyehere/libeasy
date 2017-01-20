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
#include "udp.h"

int udp_unix_socket_create(char *unix_path)
{
    int sock_fd = -1;
    struct sockaddr_un unix_addr;

    if (NULL == unix_path) {
        log_error("");
        return -1;
    }

    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0); 
    if (-1 == sock_fd) {
        log_error("create AF_UNIX failed.");
        return -1;
    }

    unlink(unix_path);

    bzero(&unix_addr, sizeof(struct sockaddr_un));
    unix_addr.sun_family = AF_UNIX;
    strncpy(unix_addr.sun_path, unix_path, sizeof(unix_addr.sun_path) - 1);

    if(bind(sock_fd, (const struct sockaddr *)&unix_addr, 
                      sizeof(struct sockaddr_un)) < 0){
        log_error("bind failed, %s", strerror(errno)); 
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

int udp_unix_socket_send(int fd, char *peer_path, char *buffer, int size)
{
    struct sockaddr_un peer_addr;
    int bytes_sent;

    if (NULL == peer_path) {
        log_error("peer_path is NULL.");
        return -1;
    }

    bzero(&peer_addr, sizeof(peer_addr));
    peer_addr.sun_family = AF_UNIX;
    strncpy(peer_addr.sun_path, peer_path, sizeof(struct sockaddr_un) - 1);

    bytes_sent = sendto(fd, buffer, size, 0, 
                       (struct sockaddr *)&peer_addr, 
                        sizeof(struct sockaddr_un)); 
    if (bytes_sent == -1) {
        log_error("unix domain socket sendto failed, %s", strerror(errno));
    }
    if (bytes_sent != size) {
        log_error("sendto: bytes sent: %d, need sent: %d", bytes_sent, size);
    }

    return bytes_sent;
}

int udp_unix_socket_recv(int fd, char *buffer, int size)
{
    struct sockaddr_un peer_addr; 
    socklen_t addr_len;
    int bytes_recv; 
    
    addr_len = sizeof(struct sockaddr_un);

    bytes_recv = recvfrom(fd, buffer, size, 0, 
                         (struct sockaddr *)&peer_addr,
                          &addr_len);
    if (bytes_recv == -1) {
        if (EAGAIN == errno) {
            return 0; 
        }
        log_error("recvfrom error, %s", strerror(errno));
        return -1;
    }
    return bytes_recv;
}

void udp_unix_socket_close(int fd)
{
    close(fd);
}
