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
#include <easy/easy.h>
#include <easy/ip_zone.h>
#include <easy/conhash.h>
#include <easy/server_conhash.h>
#include <easy/3des.h>
#include <easy/md5.h>
//#include <uuid/uuid.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


/* 
 ***************************************************************************************
 *                                  ip_zones
 ***************************************************************************************
 * */

/*
 *  参数：字符串file_name
 *
 *  返回值ip_zones_t类型
 */
int lua_ip_zones_load(lua_State *L)
{
    ip_zones_t *ip_zones = NULL;
    const char *file_name = NULL;

    if(!lua_isstring(L,1)){
        lua_pushstring(L, "incorrect argument");
        lua_error(L);
    }
    file_name = lua_tostring(L,1);
    ip_zones = ip_zones_load(file_name);

    lua_pushlightuserdata(L, ip_zones);

    return 1;
}

/*
 *  参数ip_zones_t **ip_zones
 *  返回值void
 */
int lua_ip_zones_destroy(lua_State *L)
{
    ip_zones_t *ip_zones = lua_touserdata(L, 1);

    ip_zones_destroy(&ip_zones);
    return 0;
}

/*
 *  参数1 ip_zones_t *ip_zones
 *  参数2 unsigned int ip
 *
 *  返回值 const char
 */
int lua_ip_zones_find(lua_State *L)
{
    ip_zones_t *ip_zones = lua_touserdata(L, 1);
    const char *ip = lua_tostring(L, 2); 
    const char *zone = ip_zones_find(ip_zones, ip);
    
    lua_pushstring(L, zone);
    return 1;
}


/* 
 ***************************************************************************************
 *                                  conhash
 ***************************************************************************************
 * */

/* 
 *  参数：无
 *  返回值：
 *      conhash 类型为userdata
 * */
int lua_conhash_init(lua_State *L)
{
    struct conhash_s *conhash = conhash_init(NULL);

    lua_pushlightuserdata(L, conhash);
    return 1;
}

/* 
 *
 *  参数：conhash_s 类型为userdata
 *  返回值：无
 *
 * */
int lua_conhash_fini(lua_State *L)
{
    struct conhash_s *conhash = lua_touserdata(L, 1);

    conhash_fini(conhash);
    return 0;
}


/* 
 *  参数：
 *      conhash 类型为userdata
 *      index   类型为数字
 *      iden    类型为字符串
 *      replica 类型为数字
 *  返回值：
 *      rt      0：添加成功；-1添加失败
 * */
int lua_conhash_add_node(lua_State *L)
{
    struct conhash_s *conhash = lua_touserdata(L, 1);
    int index = lua_tonumber(L, 2);
    const char *iden = lua_tostring(L, 3);
    u_int replica = lua_tonumber(L, 4);

    NODE_S *node = (NODE_S *)malloc(sizeof(NODE_S));
    conhash_set_node(node, index, iden, replica);
    int rt = conhash_add_node(conhash, node);

    if(rt == -1){
        free(node);
    }
    
    lua_pushnumber(L, rt);
    return 1;
}

/*
 *  参数：
 *      conhash 类型为userdata
 *      node    类型为userdata
 *
 *  返回值:
 *      rt      0：删除成功；-1删除失败
 */
int lua_conhash_del_node(lua_State *L)
{
    struct conhash_s *conhash = lua_touserdata(L, 1);
    const char *object = lua_tostring(L, 2);
    int len = lua_tonumber(L, 3);

    const NODE_S *node = conhash_lookup(conhash, object, len);

    int rt = conhash_del_node(conhash, (NODE_S *)node);

    lua_pushnumber(L, rt);

    return 1;
}

/*
 * 参数：
 *      conhash 类型为userdata
 *      object  类型为字符串
 *      len     类型为数字
 *
 */
int lua_conhash_lookup(lua_State *L)
{
    struct conhash_s *conhash = lua_touserdata(L, 1);
    const char *object = lua_tostring(L, 2);
    int len = lua_tonumber(L, 3);

    const NODE_S *node = conhash_lookup(conhash, object, len);

    lua_pushstring(L, node->iden);
    return 1;
}


/* 
 ***************************************************************************************
 *                                  server_conhash
 ***************************************************************************************
 * */

/* * 
 *        Name: server_conhash_create
 * Description: 创建server_conhash_t实例.
 *   Parameter: host -> 数据库主机IP.
 *              port -> 数据库端口.
 *              user -> 数据库用户名.
 *              passwd -> 数据库密码.
 *              db -> 数据库名称.
 *      Return: 返回server_conhash_t实例
 *              NULL -> 创建失败.
 */
int lua_server_conhash_create(lua_State *L)
{
    const char *lua_file = lua_tostring(L, 1); 
    //int port = lua_tonumber(L, 2); 
    //const char *user = lua_tostring(L, 3); 
    //const char *passwd = lua_tostring(L, 4); 
    //const char *db = lua_tostring(L, 5);
    if(NULL == lua_file){
        return 0;
    }

    server_conhash_t *server_conhash = server_conhash_create((char *)lua_file);

    lua_pushlightuserdata(L, server_conhash);

    return 1;
}

/** 
 *        Name: server_conhash_destroy
 * Description: 销毁实例.
 *   Parameter: server_hash.
 *      Return: 
 */
int lua_server_conhash_destroy(lua_State *L)
{
    server_conhash_t *server_conhash = lua_touserdata(L, 1);
    server_conhash_destroy(&server_conhash);

    return 0;
}

/** 
 *        Name: server_conhash_find
 * Description: 根据IDC ID和url得到相应的Server的IP.
 *   Parameter: idc -> IDC ID.
 *              url -> 视频URL.
 *      Return: 
 */
int lua_server_conhash_find(lua_State *L)
{
    server_conhash_t *server_conhash = lua_touserdata(L, 1);
    int group_id = lua_tonumber(L, 2);
    const char *url = lua_tostring(L, 3);
    const char *ip = server_conhash_find(server_conhash, group_id, (char *)url);

    lua_pushstring(L, ip);
    return 1;
}


/* 
 ***************************************************************************************
 *                                   crypt
 ***************************************************************************************
 * */

/* char转换成int*/
int ctoi(char ch)
{
    if (ch <= '9' && ch >= '0')
    {
        return ch - '0';
    }
    if (ch <= 'F' && ch >= 'A')
    {
        return ch - 'A' + 10;
    }
    if (ch <= 'f' && ch >= 'a')
    {
        return ch - 'a' + 10;
    }
    return -1;
}

/** 
 *        Name: 3des_decrypt
 * Description: 使用key将pdata解密
 *   Parameter: pdata    ->  待解密数据
 *              url     ->  密钥
 *      Return: 
 *              data    解密后的值
 */
int lua_decrypt_des3(lua_State *L)
{
    des3_context g_dc;
    const char *pdata   = lua_tostring(L, 1);
    const char *key     = lua_tostring(L, 2);
    char data[128]      = {0};
    int pdata_len       = 0;
    int key_len         = 0;
    int data_len        = 0;
    int i               = 0;

    memset(&g_dc, 0, sizeof(g_dc));
    pdata_len = strlen(pdata);
    if (pdata_len > 256)
    {
        lua_pushnil(L);
        return 1;
    }

    for(i = 0; i < pdata_len; i+=2)
    {
        int i1 = ctoi(pdata[i]);
        int i2 = ctoi(pdata[i+1]);
        data[i/2] = (i1<<4)|i2;
    }
    key_len = strlen(key);

    //设置key
    if(key_len == 8)
	{
		//des_set_key( &g_dc, key);
	}else if(key_len == 16)
	{
		des3_set_2keys( &g_dc, (unsigned char *)key, (unsigned char *)key + 8);
	}else if(key_len == 24)
	{
		des3_set_3keys( &g_dc, (unsigned char  *)key, (unsigned char *)key + 8,(unsigned char *)key + 16);
	}

    //解密
    data_len = strlen(data)/8*8;
	char *p = (char *)malloc(data_len);
	for (i = 0; i < data_len; i += 8){
		des3_decrypt(&g_dc, (unsigned char*)&data + i, (unsigned char*)p + i);
    }
	memcpy(&data, p, data_len);
    free(p);
    
    lua_pushstring(L, data);

    return 1;
}

/** 
 *        Name: 3des_encrypt
 * Description: 使用key加密pdata
 *   Parameter: pdata   ->  待加密数据
 *              key     ->  密钥
 *      Return: 
 *              加密后的值
 */
int lua_encrypt_des3(lua_State *L)
{
    des3_context g_dc;
    const char *pdata       = lua_tostring(L, 1);
    const char *key         = lua_tostring(L, 2);
    char data[128]          = {0};
    char encrypt_data[128]  = {0};
    int data_len            = 0;
    int key_len             = 0;

    memset(&g_dc, 0, sizeof(g_dc));
    key_len = strlen(key);
    memcpy(data, pdata, strlen(pdata));

    //设置key
    if(key_len == 8)
	{
		//des_set_key( &g_dc, key);
	}else if(key_len == 16)
	{
		des3_set_2keys( &g_dc, (unsigned char *)key, (unsigned char *)key + 8);
	}else if(key_len == 24)
	{
		des3_set_3keys( &g_dc, (unsigned char  *)key, (unsigned char *)key + 8,(unsigned char *)key + 16);
	}

    //加密
    data_len = strlen(data)/8*8;
	char * p = (char *)malloc(data_len);
    int i = 0;
	for (i = 0; i < data_len; i += 8){
		des3_encrypt(&g_dc, (unsigned char*)&data + i, (unsigned char*)p + i);
    }
	memcpy(&data, p, data_len);
    free(p);

    for (i = 0; i < data_len; i++){
        sprintf(encrypt_data + 2 * i, "%02x", (unsigned char)data[i]);
    }
    lua_pushstring(L, encrypt_data);

    return 1;
}

/** 
 *        Name: lua_decrypt_time_token_des3
 * Description: 使用key将data解密
 *   Parameter: data    ->  待解密数据
 *              url     ->  密钥
 *      Return: 
 *              decrypt_data data解密后的值,类型llu
 */
int lua_decrypt_time_token_des3(lua_State *L)
{
    des3_context g_dc;
    const char *data = lua_tostring(L, 1);
    const char *key = lua_tostring(L, 2);

    memset(&g_dc, 0, sizeof(g_dc));
    char decrypt_data[128]; 
    unsigned long long pdata;
    int pdata_len = 0;
    int key_len = 0;

    pdata = strtoull(data, NULL, 16);
    key_len = strlen(key);

    //设置key
    if(key_len == 8)
	{
		//des_set_key( &dc,key);
	}else if(key_len == 16)
	{
		des3_set_2keys( &g_dc, (unsigned char *)key, (unsigned char *)key + 8);
	}else if(key_len == 24)
	{
		des3_set_3keys( &g_dc, (unsigned char  *)key, (unsigned char *)key + 8,(unsigned char *)key + 16);
	}

    //解密
    pdata_len = sizeof(pdata)/8*8;
	char * p = (char*)malloc(pdata_len);
    int i = 0;
	for (i = 0; i < pdata_len; i += 8){
		des3_decrypt(&g_dc, (unsigned char*)&pdata + i, (unsigned char*)p + i);
    }
	memcpy(&pdata, p, pdata_len);
    free(p);
    sprintf(decrypt_data, "%llu", pdata);
    
    lua_pushstring(L, decrypt_data);

    return 1;
}

/** 
 *        Name: 3des_encrypt
 * Description: 获取当前时间的des3加密值
 *   Parameter: key     ->  密钥
 *      Return: 
 *              加密后的值
 */
int lua_gen_time_token_des3(lua_State *L)
{
    des3_context g_dc;
    const char *key = lua_tostring(L, 1);

    memset(&g_dc, 0, sizeof(g_dc));
    char encrypt_data[128];
    unsigned long long pdata;
    int pdata_len = 0;
    int key_len = 0;

    pdata = time(NULL);
    key_len = strlen(key);

    //设置key
    if(key_len == 8)
	{
		//des_set_key( &dc,key);
	}else if(key_len == 16)
	{
		des3_set_2keys( &g_dc, (unsigned char *)key, (unsigned char *)key + 8);
	}else if(key_len == 24)
	{
		des3_set_3keys( &g_dc, (unsigned char  *)key, (unsigned char *)key + 8,(unsigned char *)key + 16);
	}

    //加密
    pdata_len = sizeof(pdata)/8*8;
	char * p = (char*)malloc(pdata_len);
    int i = 0;
	for (i = 0; i < pdata_len; i += 8){
		des3_encrypt(&g_dc, (unsigned char*)&pdata + i, (unsigned char*)p + i);
    }
	memcpy(&pdata, p, pdata_len);
    free(p);

    sprintf(encrypt_data, "%llx", pdata);
    lua_pushstring(L, encrypt_data);

    return 1;
}


/* 
 ***************************************************************************************
 *                                  md5 crypt
 ***************************************************************************************
 */

/** 
 *        Name: gen_md5
 * Description: 生成字符串的md5值
 *   Parameter: data    ->  字符串
 *      Return: 
 *              md5_string  ->  md5值
 */
int lua_gen_md5(lua_State *L)
{
    const char *data = lua_tostring(L, 1);
    unsigned char digest[16];
    char md5_string[34] = {0};
    int i;
    md5_state_t md5;

    md5_init(&md5);
    md5_append(&md5, (unsigned char*)data, strlen(data));
    md5_finish(&md5, digest);

    for(i = 0; i < 16; i++){
        sprintf(md5_string+2*i, "%02x", digest[i]);
    }

    lua_pushstring(L, md5_string);

    return 1;
}

/* 
 ***************************************************************************************
 *                                  uuid
 ***************************************************************************************
 */

/** 
 *        Name: gen_uuid
 * Description: 生成uuid
 *   Parameter: 无
 *      Return: 
 *              uuid_string 
 */
/**int lua_gen_uuid(lua_State *L)
{
    uuid_t uuid;
    char uuid_str[32] = {0};
    int i = 0;
    unsigned char *p = NULL;

    uuid_generate(uuid);
    p = uuid;
    
    for(i = 0; i < 16; i++, p++)
    {
        snprintf(uuid_str + i * 2, 3, "%02x", *p);
    }

    lua_pushstring(L, uuid_str);

    return 1;
}
*/
/* 
 ***************************************************************************************
 *                                  process_lock
 ***************************************************************************************
 * */

/** 
 *        Name: process_lock_create
 * Description: 创建进程间同步的互斥锁。
 *   Parameter: file   -> 用来生成互斥锁key的文件。
 *              sub_id -> 和file一起生成互斥锁key。
 *             
 *      Return: 返回互斥锁，失败返回nil。
 */
int lua_process_lock_create(lua_State *L)
{
    process_lock_t *pm = NULL;
    const char *file = NULL;
    int sub_id = -1;
    
    file = lua_tostring(L, 1);
    sub_id = lua_tonumber(L, 2);

    pm = process_lock_create((char *)file, sub_id);
    if(NULL == pm){
        lua_pushnil(L);
        lua_pushstring(L, "create lock failed");
        return 2;
    }

    lua_pushlightuserdata(L, pm);
    return 1;
}

/** 
 *        Name: process_lock_init
 * Description: 初始化进程间同步的互斥锁, 
 *              不同的进程之间只能调用一次。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 初始化成功。
 *              0 -> 初始化失败。
 */
int lua_process_lock_init(lua_State *L)
{
    process_lock_t *pm = NULL;
    int rt = -1;
    
    pm = lua_touserdata(L, 1);
    rt = process_lock_init(pm);
    if(0 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "semctl failed");
        return 2;
    }

    lua_pushstring(L, "ok");
    return 1;
}

/** 
 *        Name: process_try_lock
 * Description: 试图执行锁操作，如果成功则锁住。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1  -> 锁成功。
 *              0  -> 没有锁住。
 *              -1 -> 锁失败。
 */
int lua_process_try_lock(lua_State *L)
{
    process_lock_t *pm = NULL;
    int rt = -1;

    pm = lua_touserdata(L, 1);
    rt = process_try_lock(pm);
    if(0 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "EAGAIN");
        return 2;
    }else if(-1 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "semop try lock failed");
        return 2;
    }

    lua_pushstring(L, "ok");
    return 1;
}

/** 
 *        Name: process_lock
 * Description: 执行锁操作。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 锁成功。
 *              0 -> 锁失败。
 */
int lua_process_lock(lua_State *L)
{
    process_lock_t *pm = NULL;
    int rt = -1;

    pm = lua_touserdata(L, 1);
    rt = process_lock(pm);
    if(0 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "semop for lock failed");
        return 2;
    }

    lua_pushstring(L, "ok");
    return 1;
}

/** 
 *        Name: process_unlock
 * Description: 执行解锁操作。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 解锁成功。
 *              0 -> 解锁失败。
 */
int lua_process_unlock(lua_State *L)
{
    process_lock_t *pm = NULL;
    int rt = -1;

    pm = lua_touserdata(L, 1);
    rt = process_unlock(pm);
    if(0 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "semop for unlock failed");
        return 2;
    }

    lua_pushstring(L, "ok");
    return 1;
}

/** 
 *        Name: process_lock_release
 * Description: 销毁锁实例,但并不删除全局的信号灯。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 销毁锁实例成功。
 *              0 -> 销毁锁实例失败。
 */
int lua_process_lock_release(lua_State *L)
{
    process_lock_t *pm = NULL;

    pm = lua_touserdata(L, 1);
    process_lock_release(&pm);
    
    lua_pushstring(L, "ok");
    return 1;
}

/** 
 *        Name: process_lock_delete
 * Description: 删除全局的信号灯，应该在process_lock_release之前调用。
 *   Parameter: pm -> 锁实例。
 *             
 *      Return: 1 -> 删除全局信号灯成功。
 *              0 -> 删除全局信号灯失败。
 */
int lua_process_lock_delete(lua_State *L)
{
    process_lock_t *pm = NULL;
    int rt = -1;

    pm = lua_touserdata(L, 1);
    rt = process_lock_delete(pm);
    if(0 == rt){
        lua_pushnil(L);
        lua_pushstring(L, "delete global sem failed");
        return 2;
    }
    
    lua_pushstring(L, "ok");
    return 1;
}

const luaL_Reg easy_lua[] = 
{
    {"ip_zones_load",           lua_ip_zones_load},
    {"ip_zones_destroy",        lua_ip_zones_destroy},
    {"ip_zones_find",           lua_ip_zones_find},

    {"conhash_init",            lua_conhash_init},
    {"conhash_fini",            lua_conhash_fini},
    {"conhash_add_node",        lua_conhash_add_node},
    {"conhash_del_node",        lua_conhash_del_node},
    {"conhash_lookup",          lua_conhash_lookup},

    {"server_conhash_create",   lua_server_conhash_create},
    {"server_conhash_find",     lua_server_conhash_find},
    {"server_conhash_destroy",  lua_server_conhash_destroy},

    {"decrypt_des3",            lua_decrypt_des3},
    {"encrypt_des3",            lua_encrypt_des3},
    {"decrypt_time_token_des3", lua_decrypt_time_token_des3},
    {"gen_time_token_des3",     lua_gen_time_token_des3},
    {"gen_md5",                 lua_gen_md5},
//    {"gen_uuid",                lua_gen_uuid},

    {"process_lock_create",     lua_process_lock_create},
    {"process_lock_init",       lua_process_lock_init},
    {"process_try_lock",        lua_process_try_lock},
    {"process_lock",            lua_process_lock},
    {"process_unlock",          lua_process_unlock},
    {"process_lock_release",    lua_process_lock_release},
    {"process_lock_delete",     lua_process_lock_delete},

    {NULL,NULL}
};

LUALIB_API int luaopen_easy_lua(lua_State *L)
{
    //luaL_register(L,"easy_lua", easy_lua);
    luaL_newlib(L, easy_lua);
    return 0;
}

