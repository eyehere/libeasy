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
#include "ip_zone.h"

static char *read_ip_zones_from_file(const char *file_name, off_t *size);
static int ip_zones_add(ip_zones_t *ip_zones, char *line);
static ip_zone_t *rbtree_search(struct rb_root *root, const char *ip);
static int rbtree_insert(struct rb_root *root, ip_zone_t *data);

ip_zones_t *ip_zones_load(const char *file_name)
{
    ip_zones_t *ip_zones        = NULL;
    char       *ip_zones_buffer = NULL;
    off_t       ip_zones_size   = 0;
    int         i               = 0;

    ip_zones_buffer = read_ip_zones_from_file(file_name, &ip_zones_size); 
    if (NULL == ip_zones_buffer) {
        return NULL; 
    }

    ip_zones = (ip_zones_t *)malloc(sizeof(ip_zones_t));
    if (NULL == ip_zones) {
        log_error("malloc for ip_zones failed, %s.", strerror(errno));
        free(ip_zones_buffer);
        return NULL;
    }
    bzero(ip_zones, sizeof(ip_zones_t));
    ip_zones->ip_zone_tree = RB_ROOT;

    while (i < ip_zones_size) {
        char *line  = &ip_zones_buffer[i];
        int   count = 0;
        char *str   = NULL;
        int   blank = 1;

        /* 寻找行结束符号 */ 
        while (i + count < ip_zones_size) {
            if (ip_zones_buffer[i + count] == '\r') {
                if (i + count >= ip_zones_size - 1) {
                    log_error("ip zones illegal format.");
                    free(ip_zones_buffer);
                    ip_zones_destroy(&ip_zones);
                    return NULL;
                }
                if (ip_zones_buffer[i + count + 1] == '\n') {
                    ip_zones_buffer[i + count] = 0;
                    i = i + count + 2;
                    break;
                }
                else{
                    free(ip_zones_buffer); 
                    ip_zones_destroy(&ip_zones);
                    return NULL;
                }
            }
            else if (ip_zones_buffer[i + count] == '\n') {
                ip_zones_buffer[i + count] = 0;
                i = i + count + 1;
                break;
            }
            count++;
        }

        /* 跳过开头的空格 */
        str = line;
        while(0 != *str){
            if(' ' != *str){
                blank = 0;
                break;
            }
            str++;
        }

        if(blank){
            continue;
        }
        
        /* 如果第一个字母是#，跳过继续下一行 */
        if('#' == *str){
            continue;
        }

        if(!ip_zones_add(ip_zones, str)){
            log_error("ip_zones_add line[%s] failed.", line);
            free(ip_zones_buffer);
            ip_zones_destroy(&ip_zones);
            return NULL;
        }
    }

    free(ip_zones_buffer);
    return ip_zones;
}

void ip_zones_destroy(ip_zones_t **ip_zones)
{
    ip_zones_t     *p    = NULL;
    struct rb_node *node = NULL;

    if(NULL == ip_zones){
        return;
    }

    p = *ip_zones;

    node = rb_first(&(p->ip_zone_tree));

    while (node) {
        struct rb_node *next    = NULL;
        ip_zone_t      *ip_zone = NULL;

        next = rb_next(node);
        rb_erase(node, &(p->ip_zone_tree));
        ip_zone = container_of(node, ip_zone_t, node);
        free(ip_zone);
        ip_zone = NULL;

        node = next;
    }

    free(p);

    *ip_zones = NULL;
}

const char *ip_zones_find(ip_zones_t *ip_zones, const char *ip)
{
    ip_zone_t *ip_zone;
    ip_zone = rbtree_search(&(ip_zones->ip_zone_tree), ip);
    if (NULL == ip_zone) {
        return NULL;
    }
    return ip_zone->zone_name;
}

void ip_zones_dump(ip_zones_t *ip_zones)
{
    struct rb_node *node;
    char buffer[128];

    for (node = rb_first(&ip_zones->ip_zone_tree); node; node = rb_next(node)){
        ip_zone_t *ip_zone = container_of(node, ip_zone_t, node);
        bzero(buffer, sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1, "%s;|%s\n", ip_zone->subnet, ip_zone->zone_name);
        printf("%s", buffer);
    }
}

static ip_zone_t *rbtree_search(struct rb_root *root, const char *str_ip)
{
    struct rb_node *node = root->rb_node;
    unsigned int    ip   = ntohl(inet_addr(str_ip));

    while (node) {
        ip_zone_t *ip_zone = container_of(node, ip_zone_t, node);

        if (ip < ip_zone->start) {
            node = node->rb_left;
        }
        else if (ip >= ip_zone->end) {
            node = node->rb_right;
        }
        else {
            return ip_zone;
        }
    }
    return NULL;
}

static int rbtree_insert(struct rb_root *root, ip_zone_t *data)
{
    struct rb_node **new   = &(root->rb_node);
    struct rb_node *parent = NULL;

    while (*new) {
        ip_zone_t *this = container_of(*new, ip_zone_t, node);

        parent = *new;
        if (data->start < this->start) {
            new = &((*new)->rb_left);
        }
        else if (data->end >= this->end) {
            new = &((*new)->rb_right);
        }
        else {
            log_error("rbtree_insert error, %s", data->zone_name);
            return 0;
        }
    }

    /* Add new node and rebalance tree. */ 
    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);

    return 1;
}

static char *read_ip_zones_from_file(const char *file_name, off_t *size)
{
    char *ip_zones_buffer = NULL;
    off_t read_len = 0;
    FILE *fp = NULL;
    
    read_len = file_get_length(file_name);
    if (read_len <= 0) {
        return NULL;
    }

    fp = fopen(file_name, "r");
    if (NULL == fp) {
        return NULL;
    }

    ip_zones_buffer = (char *)malloc(read_len);
    if (NULL == ip_zones_buffer) {
        fclose(fp);
        return NULL;
    }

    *size = fread(ip_zones_buffer, 1, read_len, fp); 

    if (*size <= 0 || *size != read_len) {
        fclose(fp);
        free(ip_zones_buffer);
        return NULL;
    }

    fclose(fp);
    return ip_zones_buffer;
}

static int ip_zones_add(ip_zones_t *ip_zones, char *line)
{
    char *ip   = NULL; 
    char *zone = NULL;
    char *str  = NULL;
    char *netmask = NULL;
    ip_zone_t *ip_zone = NULL;

    ip_zone = (ip_zone_t *)malloc(sizeof(ip_zone_t));
    if (NULL == ip_zone) {
        log_error("malloc for ip_zone failed.");
        return 0;
    }
    bzero(ip_zone, sizeof(ip_zone_t));
    
    /* 定位IP ZONE的分割位置 */
    str = line;
    ip = line;
    while(0 != *str){
        if(';' == *str && '|' == *(str + 1)){
            zone = str + 2;
            *str = 0;
            break;
        }
        str++;
    }
    
    if(NULL == zone){
        log_error("error line, %s.", line);
        free(ip_zone);
        return 0;
    }

    /* 去掉行尾的回车符和空格 */
    str = zone;
    while(0 != *str){
        if(' ' == *str || '\r' == *str || '\n' == *str){
            *str = 0;
            break;
        }
        str++; 
    }

    memcpy(ip_zone->subnet, ip, zone - ip - 2);
    
    /* 定位IP和子网掩码的分割位置 */
    str = ip; 
    while(0 != *str){
        if('/' == *str){
            netmask = str + 1;
            *str = 0;
            break;
        }
        str++; 
    }

    if(NULL == netmask){
        log_error("error line, %s.", line);
        free(ip_zone);
        return 0;
    }

    /* 得到整数子网掩码 */
    ip_zone->netmask = atoi(netmask);

    /* 计算IP段起始地址 */
    if (ip_zone->netmask == 0) {
        ip_zone->start = 0;
        ip_zone->end   = 0xffffffff;
    }
    else {
        ip_zone->start = ntohl(inet_addr(ip)); 
        ip_zone->end   = ip_zone->start + (1 << (32 - ip_zone->netmask));
    }

    /* 拷贝区域名称 */
    strncpy(ip_zone->zone_name, zone, MAX_ZONE_NAME_SIZE - 1);
    ip_zones->count++;

    rbtree_insert(&ip_zones->ip_zone_tree, ip_zone);
    return 1;
}
