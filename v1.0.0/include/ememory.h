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
#ifndef _EMEMORY_H_
#define _EMEMORY_H_
#include "ecommon.h"

#ifndef MALLOC_DEBUG
#define MALLOC_WRAPPER(size) malloc(size)
#else
#define MALLOC_WRAPPER(size) malloc_wrapper(size)
#endif

/** 
 *        Name: malloc_wrapper
 * Description: 用来debug malloc用的.
 *   Parameter: size -> 分配内存的大小.
 *      Return: 分配的内存的首地址.
 */
void *malloc_wrapper(int size);
#endif
