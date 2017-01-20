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
#include "server_conhash.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static void group_conhash_free(void *data);
static void ip_free(void *data);
static int group_load(lua_State *L, server_conhash_t *server_conhash);
static int group_server_load(lua_State *L, server_conhash_t *server_conhash, 
        struct conhash_s *conhash, int seq, int id);

void load_data(lua_State *L);
int get_group_count(lua_State *L);
int get_group_id(lua_State *L, int i);
int get_server_count_by_seq(lua_State *L, int group_id);
void get_server_by_seq(lua_State *L, int seq, int n, char *ip, int ip_size, char *hash_ip, int hash_ip_size);

server_conhash_t *server_conhash_create(char *lua_file)
{
    server_conhash_t *server_conhash = NULL;
    lua_State *L = luaL_newstate();

    server_conhash = (server_conhash_t *)malloc(sizeof(server_conhash_t));
    if(NULL == server_conhash){
        log_error("malloc for server_conhash failed."); 
        return NULL;
    }
    bzero(server_conhash, sizeof(server_conhash_t));

    /* 创建一致性哈希实例的哈希表 */
    server_conhash->conhashs = hashmap_new(1024, 0, GH_COPYKEYS, group_conhash_free);
    if(NULL == server_conhash->conhashs){
        log_error("hashmap_new for group conhashs failed.");
        goto error;
    }

    /* 用来存储haship到ip的映射 */
    server_conhash->ips = hashmap_new(1024, 0, GH_COPYKEYS, ip_free);
    if(NULL == server_conhash->ips){
        log_error("hashmap_new for ips failed.");
        goto error;
    }

    //初始化lua
    luaL_openlibs(L);
    if(0 != luaL_dofile(L, lua_file)){
        log_error("load lua file error\n");
        goto error; 
    }
    load_data(L);

    if(!group_load(L, server_conhash)){
        log_error("group_load failed.");
        goto error;
    }

    return server_conhash;

error:
    lua_close(L);

    if(server_conhash){
        server_conhash_destroy(&server_conhash);
    }

    return NULL;
}

void server_conhash_destroy(server_conhash_t **server_conhash)
{
    if(NULL == server_conhash || NULL == *server_conhash){
        return;
    }

    if((*server_conhash)->conhashs){
        hashmap_delete((*server_conhash)->conhashs);
    }

    if((*server_conhash)->ips){
        hashmap_delete((*server_conhash)->ips);
    }

    free(*server_conhash);
    *server_conhash = NULL;
    return;
}

char *server_conhash_find(server_conhash_t *server_conhash, int group_id, char *url)
{
    struct conhash_s *conhash = NULL;
    const NODE_S *node = NULL;
    char str_id[32] = {0};
    char *ip = NULL;
    char ip_key[64] = {0};

    if(NULL == server_conhash || NULL == server_conhash->conhashs){
        return NULL;
    }

    if(group_id < 0 || NULL == url || strlen(url) == 0){
        return NULL;
    }

    snprintf(str_id, sizeof(str_id) - 1, "%d", group_id);
    printf("group_id: %s.\n", str_id);
    conhash = hashmap_find(server_conhash->conhashs, str_id);
    if(NULL == conhash){
        return NULL;
    }

    node = conhash_lookup(conhash, url, strlen(url));
    
    snprintf(ip_key, sizeof(ip_key) - 1, "%d_%s", group_id, (char *)(node->iden));
    ip = hashmap_find(server_conhash->ips, ip_key);
    if(ip){
        return ip;
    }

    return (char *)node->iden;
}

//==============================================================================

static void group_conhash_free(void *data)
{
    struct conhash_s *conhash = (struct conhash_s *)data;
    conhash_fini(conhash); 
}

static void ip_free(void *data)
{
    char *ip = (char *)data;
    free(ip);
}

static int group_load(lua_State *L, server_conhash_t *server_conhash)
{
    int i = 0;
    int id = 0;
    int count = get_group_count(L);
    char str_id[32] = {0};

    for(i = 1; i <= count; i++){
        struct conhash_s *conhash = conhash_init(NULL); 
        if(NULL == conhash){
            log_error("conhash_init failed.");
            return 0;
        }

        id = get_group_id(L, i);
        if(!id){
            log_error("id is NULL in group_load.");
            return 0;
        }

        if(!group_server_load(L, server_conhash, conhash, i, id)){
            log_error("load group server failed, group id: %d.", id);
            return 0;
        }
        bzero(str_id, sizeof(str_id));
        snprintf(str_id, sizeof(str_id) - 1, "%d", id);
        hashmap_add(server_conhash->conhashs, str_id, conhash);
    }
    return 1;
}

static int group_server_load(lua_State *L, server_conhash_t *server_conhash, 
        struct conhash_s *conhash, int seq, int id)
{
    int count;
    int j;
    char ip[32] = {0};
    char hash_ip[32] = {0};
    char ip_key[64] = {0};

    count = get_server_count_by_seq(L, seq);
    for(j = 1; j <= count; j++){
        bzero(ip, sizeof(ip));
        bzero(hash_ip, sizeof(hash_ip));
        get_server_by_seq(L, seq, j, ip, sizeof(ip), hash_ip, sizeof(hash_ip));
        
        //printf("group:%d, ip:%s, hash_ip:%s\n", seq, ip, hash_ip);
        NODE_S *node = (NODE_S *)malloc(sizeof(NODE_S));
        if(NULL == node){
            log_error("create node failed.");
            return 0;
        }
        if(NULL == hash_ip || strlen(hash_ip) <= 0){
            conhash_set_node(node, j - 1, ip, 500);
        }
        else{
            char *_ip = (char *)malloc(strlen(ip) + 1);
            if(NULL == _ip){
                log_error("malloc for _ip failed.");
                return 0;
            }
            bzero(_ip, strlen(ip) + 1);
            strcpy(_ip, ip);

            conhash_set_node(node, j - 1, hash_ip, 500);

            /* 如果有hash ip，则需要将hash ip加入到哈希表中 */
            snprintf(ip_key, sizeof(ip_key) - 1, "%d_%s", id, hash_ip);
            hashmap_add(server_conhash->ips, ip_key, _ip);
        }
        conhash_add_node(conhash, node);
    }

    return 1;
}

void load_data(lua_State *L){
    lua_getglobal(L, "load_data");
    if(lua_pcall(L, 0, 0, 0)){
        log_error("failed to load data\n");
    }else {
        log_info("load data successful\n");
    }
    return;
}

int get_group_count(lua_State *L){
    int count = 0;
    lua_getglobal(L, "get_group_count"); 
    if(lua_pcall(L, 0, 1, 0)){
        log_error("get_group_count error\n");
    }
    count = lua_tonumber(L, 1);

    lua_pop(L, 1);
    return count;
}

int get_group_id(lua_State *L, int i){
    int id = 0;
    lua_getglobal(L, "get_group_id"); 
    lua_pushnumber(L, i);
    if(lua_pcall(L, 1, 1, 0)){
        log_error("get_group_id error\n");
    }
    id = lua_tonumber(L, 1);

    lua_pop(L, 1);
    return id;
}

int get_server_count_by_seq(lua_State *L, int group_id){
    int count = 0;
    lua_getglobal(L, "get_server_count_by_seq");
    lua_pushnumber(L, group_id);
    if(lua_pcall(L, 1, 1, 0)){
        log_error("get_server_count_by_seq error\n");
    }
    count = lua_tonumber(L, 1);

    lua_pop(L, 1);
    return count;
}

void get_server_by_seq(lua_State *L, int i, int n, char *ip, int ip_size, char *hash_ip, int hash_ip_size){
    const char *_ip = NULL;
    const char *_hash_ip = NULL;
    lua_getglobal(L, "get_server_by_seq"); 
    lua_pushnumber(L, i);
    lua_pushnumber(L, n);
    if(lua_pcall(L, 2, 2, 0)){
        log_error("get_server_by_seq error\n");
    }

    _ip = lua_tostring(L, 1);
    _hash_ip = lua_tostring(L, 2);

    snprintf(ip, ip_size - 1, "%s", _ip);
    snprintf(hash_ip, hash_ip_size - 1, "%s", _hash_ip);

    lua_pop(L, 2);
}

