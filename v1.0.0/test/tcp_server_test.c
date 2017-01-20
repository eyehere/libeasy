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
#include <easy/tcp_server.h>
#include <easy/easy.h>

static int  install_signals(void);
static void signal_handle(int signum);
static tcp_server_t *server = NULL;

int main(int argc, char **argv) 
{
    install_signals();
    server = tcp_server_create(5000, 10000);
    if (NULL == server) {
        log_error("tcp_server_create failed.");
        return 0;
    }

    tcp_server_set_rcv_cb(server, cb);
    tcp_server_set_snd_cb(server, cb);
    tcp_server_run(server); 

    tcp_server_destroy(server);

    return 1;
}

static int install_signals(void){
    if(SIG_ERR == signal(SIGINT, signal_handle)){
        log_error("Install SIGINT fails.\n");
        return 0;
    }   
    if(SIG_ERR == signal(SIGTERM, signal_handle)){
        log_error("Install SIGTERM fails.\n");
        return 0;
    }   
    if(SIG_ERR == signal(SIGSEGV, signal_handle)){
        log_error("Install SIGSEGV fails.\n");
        return 0;
    }   
    if(SIG_ERR == signal(SIGBUS, signal_handle)){
        log_error("Install SIGBUS fails.\n");
        return 0;
    }   
    if(SIG_ERR == signal(SIGQUIT, signal_handle)){
        log_error("Install SIGQUIT fails.\n");
        return 0;
    }   
    if(SIG_ERR == signal(SIGCHLD, signal_handle)){
        log_error("Install SIGCHLD fails.\n");
        return 0;
    }

    return 1;
}

static void signal_handle(int signum){
    //SIGINT triggered by CTRL-C
    //SIGTERM triggered by kill
    if(SIGTERM == signum){
        log_info("recv kill signal, va_server will exit normally\n");
        tcp_server_stop(server);
    }
    else if(SIGINT == signum){
        log_info("recv CTRL-C signal, va_server will exit normally\n");
        tcp_server_stop(server);
    }
    else if(SIGCHLD == signum){
        log_info("recv SIGCHLD signal[%d].\n", signum);
    }
    else{
        log_info("receive signal: %d\n", signum);
        exit(0);
    }
}
