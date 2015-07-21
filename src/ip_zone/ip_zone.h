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
#ifndef IP_ZONE_H_
#define IP_ZONE_H_
#include "easy.h"
#include "rbtree.h"

#define MAX_ZONE_NAME_SIZE 64
#define MAX_SUBNET_SIZE    24

/* 每一个区域对应一段IP范围 */
typedef struct _ip_zone_t {
    struct rb_node node;
    unsigned int  start;
    unsigned int  end;
    unsigned char netmask;
    char subnet[MAX_SUBNET_SIZE];
    char zone_name[MAX_ZONE_NAME_SIZE];
}ip_zone_t;
    
/* 所有的IP区域映射 */
typedef struct _ip_zones_t {
    struct rb_root ip_zone_tree;
    unsigned int count; 
}ip_zones_t;

/** 
 *        Name: ip_zones_load
 * Description: 从文件加载IP库.
 *   Parameter: file_name -> IP库文件.
 *              
 *      Return: 返回IP库实例, 失败返回NULL.
 */
ip_zones_t *ip_zones_load(const char *file_name);

/** 
 *        Name: ip_zones_destroy
 * Description: 释放IP库资源.
 *   Parameter: ip_zones -> IP库实例.
 *      Return: 
 */
void ip_zones_destroy(ip_zones_t **ip_zones);

/** 
 *        Name: ip_zones_find
 * Description: 根据IP查找区域, ip是主机字节序的整数.
 *   Parameter: ip_zones -> IP库实例.
 *              ip -> ip字符串.
 *      Return: ip所在的区域.
 */
const char *ip_zones_find(ip_zones_t *ip_zones, const char *ip_str);

/** 
 *        Name: ip_zones_dump
 * Description: 输出所有的ip库
 *   Parameter: ip_zones -> IP库实例.
 *
 *      Return: 
 */
void ip_zones_dump(ip_zones_t *ip_zones);

#endif
