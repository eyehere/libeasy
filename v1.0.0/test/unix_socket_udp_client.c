/*
  +----------------------------------------------------------------------+
  | libeasy                                                              |
  +----------------------------------------------------------------------+
  | this source file is subject to version 2.0 of the apache license,    |
  | that is bundled with this package in the file license, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/license-2.0.html                      |
  | if you did not receive a copy of the apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | yiming_6weijun@163.com so we can mail you a copy immediately.        |
  +----------------------------------------------------------------------+
  | Author: Weijun Lu  <yiming_6weijun@163.com>                          |
  +----------------------------------------------------------------------+
*/
#include <easy/udp.h>

#define UNIX_DOMAIN_PATH "/tmp/unix_domain_path_client.sock"
#define UNIX_DOMAIN_SERVER_PATH "/tmp/unix_domain_path_server.sock"

int main(void) 
{
    int sock_fd = -1;
    
    log_set_level(LOG_LEVEL_DEBUG);
    sock_fd = udp_unix_socket_create(UNIX_DOMAIN_PATH);
    if (sock_fd == -1) {
        printf("udp_unix_socket_create failed.");
        return 0;
    }

    set_non_blocking(sock_fd); 
    while(1) {
        int bytes_sent = -1;
        bytes_sent = udp_unix_socket_send(sock_fd, UNIX_DOMAIN_SERVER_PATH, "hello", sizeof("hello") - 1);

        log_debug("bytes_sent: %d", bytes_sent);
    }

    udp_unix_socket_close(sock_fd); 

    return 1;
}

