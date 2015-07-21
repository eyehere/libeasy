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
#include "network_util.h"

int set_non_blocking(int sock_fd){
    int opts;
    opts = fcntl(sock_fd,F_GETFL);
    if(opts < 0){
        log_error("fcntl(sock_fd,GETFL), %s", strerror(errno));
        return 0;
    } 

    opts = opts | O_NONBLOCK;
    if(fcntl(sock_fd,F_SETFL,opts)<0){
        log_error("fcntl(sock_fd,SETFL,opts), %s", strerror(errno));
        return 0;
    } 

    return 1;
}

int set_reuse_addr(int sock_fd){
    int opt_val       = 1;
    socklen_t opt_len = sizeof(int);
    int ret;
    
    ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, opt_len);
    return ret < 0 ? 0 : 1;
}

int nic_get_ip(char *name, char *ip, int ip_size)
{
    int fd = -1; 
    struct ifreq ifr;

    bzero(ip, ip_size);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        log_error("nic_get_addr: create socket failed.");
        return 0;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name , name);

    ioctl(fd, SIOCGIFADDR, &ifr);
    strncpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), ip_size - 1);   

    return 1;
}
