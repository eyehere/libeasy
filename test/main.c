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
#include <easy/easy.h>
#include <easy/hashmap.h>
#include <easy/ip_zone.h>
#include <easy/server_conhash.h>

#if 0
void myfree ( void * p )
{
	free(p);
}
#endif

/*
*       Hash test program
*/
int main ( int argc, char ** argv )
{
#if 0
    printf("%d\n", is_valid_ip("256.0.0.1"));
    return 0;
#endif

#if 0
    server_conhash_t *server_conhash = NULL;
    char *ip = NULL;

    server_conhash = server_conhash_create("/opt/soft/cdn_dispatcher/cdn_dispatcher/lua/load_server_conhash_data.lua");

    ip = server_conhash_find(server_conhash, 104, "/videos/comic/20120530/9731e0fbb90f2e3da9eafd99a786daa8.f4v");
    if(ip){
        printf("%s\n", ip);
    }
    
    server_conhash_destroy(&server_conhash);
#endif

    //printf("cmp %s %s.\n", argv[1], argv[2]);
    //printf("%d.\n", str_cmp(argv[1], strlen(argv[1]), argv[2], strlen(argv[2])));
#if 0
    shm_t *shm = NULL;

    create_file_if_not_existed("./test.shm");

    shm = shm_create("./test.shm", 1024 * 1024);
    if(shm == NULL){
        return 0;
    }

    //shm_delete(shm);

    shm_destroy(&shm);
#endif
    char buffer[2048] = {0};
    int i = 0;

    for (i = 0; i < 2048; i++) {
        buffer[i] = 'a';
    }

    log_set_level(LOG_LEVEL_DEBUG);
    log_debug("%s", buffer);
    log_info("%s", buffer);
    log_warn("%s", buffer);
    log_error("%s", buffer);

    
#if 0
    ip_zones_t *ip_zones = NULL;

    ip_zones = ip_zones_load("/opt/soft/cdn_dispatcher/data/ip_zone.data");

    if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        char buffer[1024];

        while( !feof(fp) ) {
            char *str;
            bzero(buffer, sizeof(buffer));
            fgets(buffer, sizeof(buffer), fp);
            str = buffer; 

            while(*str != 0){
                if (*str == '\n'){
                    *str = 0;
                    break;
                }
                str++;
            }

            if (strlen(buffer) > 0) {
                printf("%s,%s\n", buffer, ip_zones_find(ip_zones, buffer));
            }
        }

        fclose(fp);
    }
    else {
        printf("%s,%s\n", "223.248.0.0", ip_zones_find(ip_zones, "223.248.0.0"));
    }

    //ip_zones_dump(ip_zones);
    ip_zones_destroy(&ip_zones);
#endif

#if 0
    int i = 0;
    char buffer[905] = {0};
    for(i = 0; i < (int)sizeof(buffer) - 1; i++){
        buffer[i] = i % 10 + 0x30;
    }

    log_set_level(LOG_LEVEL_DEBUG); 
    //log_set_syslog("127.0.0.1", 514, SYSLOG_LOCAL0, NULL, "test"); 
    log_debug("test syslog-ng %d %s.", 100, "hello");
    log_info("test syslog-ng %d %s.", 100, "hello");
    log_warn("test syslog-ng %d %s.", 100, "hello");
    log_error("test syslog-ng %d %s.", 100, "hello");
    log_close();
#endif

#if 0
    int         i;
    hashmap_t  *t;
    hashmap_node_t *n;
    char str[256],*p;
    int  num=100;

    if( argc > 1 )
        num = atoi(argv[1]);

    /* Create a Hash Table */
    t = hashmap_new( 1000, 0 , GH_COPYKEYS , myfree  );

    /* Add Nodes to the Hash Table */
    for(i=0;i<num;i++)
    {
        snprintf(str, sizeof(str), "KeyWord%d",i+1);
        str[sizeof(str) - 1] = '\0';
        hashmap_add( t, str,  strdup(str));
    }

    /* Find and Display Nodes in the Hash Table */
    printf("\n** FIND KEY TEST\n");

    for(i=0;i<num;i++)
    {
        snprintf(str, sizeof(str), "KeyWord%d",i+1);
        str[sizeof(str) - 1] = '\0';

        p = (char*) hashmap_find( t, str );

        printf("Hash-key=%*s, data=%*s\n", (int)strlen(str),str, (int)strlen(str), p );
    }

    /* Display All Nodes in the Hash Table */
    printf("\n** FINDFIRST / FINDNEXT TEST\n");

    for( n = hashmap_findfirst(t); n; n = hashmap_findnext(t) )
    {
        printf("hash-findfirst/next: key=%s, data=%s\n", (char *)n->key, (char *)n->data );

        // hashing code frees user data using 'myfree' above ....
        if( hashmap_remove(t,n->key) )
            printf("Could not remove the key node\n");
        else
            printf("key node removed\n");
    }

    /* Free the table and it's user data */
    printf("****sfghash_delete\n");
    hashmap_delete( t );

    printf("\nnormal pgm finish\n\n");
#endif

    return 1;
}
