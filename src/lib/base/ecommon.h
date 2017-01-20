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
#ifndef _ECOMMON_H_
#define _ECOMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/un.h>
#if defined(__FreeBSD__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/event.h>
#else
#include <linux/types.h>
#include <malloc.h>
#include <sys/epoll.h>
#endif
#include <assert.h>
#include <inttypes.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define _XOPEN_SOURCE 600

#define ALIGN_PTR(ptr, align)                                \
     (void *)(((long)ptr + (align - 1)) & ~((long)align - 1))

#define ALIGN_INT(ptr, align)                                \
     (((int)ptr + (align - 1)) & ~((int)align - 1))

#endif
