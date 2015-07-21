OBJS_DIR     = objs

EASY_OBJS    = $(OBJS_DIR)/elog.o          \
			   $(OBJS_DIR)/etime.o         \
			   $(OBJS_DIR)/estring.o       \
			   $(OBJS_DIR)/eother.o        \
			   $(OBJS_DIR)/equeue.o        \
			   $(OBJS_DIR)/emem_pool.o     \
			   $(OBJS_DIR)/ethread_pool.o  \
			   $(OBJS_DIR)/ememory.o       \
			   $(OBJS_DIR)/eprocess_lock.o \
			   $(OBJS_DIR)/esyslog.o       \
			   $(OBJS_DIR)/eproperty.o     \
			   $(OBJS_DIR)/enetwork.o      \
			   $(OBJS_DIR)/eshm.o          \
			   $(OBJS_DIR)/ebuffer.o

HASHMAP_OBJS = $(OBJS_DIR)/hash_fcn.o      \
			   $(OBJS_DIR)/hashmap.o       \
			   $(OBJS_DIR)/prime_table.o

CONHASH_OBJS = $(OBJS_DIR)/conhash.o       \
			   $(OBJS_DIR)/conhash_inter.o \
			   $(OBJS_DIR)/conhash_util.o  \
			   $(OBJS_DIR)/crc32.o         \
			   $(OBJS_DIR)/util_rbtree.o

CRYPT_OBJS	 = $(OBJS_DIR)/3des.o \
			   $(OBJS_DIR)/md5.o	

SERVER_CONHASH_OBJS = $(OBJS_DIR)/server_conhash.o

RBTREE_OBJS  = $(OBJS_DIR)/rbtree.o

IP_ZONE_OBJS = $(OBJS_DIR)/ip_zone.o

NETWORK_OBJS = $(OBJS_DIR)/udp.o          \
               $(OBJS_DIR)/network_util.o 

OBJS         = $(EASY_OBJS)           \
	   		   $(HASHMAP_OBJS)        \
	  	       $(CONHASH_OBJS)        \
			   $(IP_ZONE_OBJS)        \
			   $(SERVER_CONHASH_OBJS) \
			   $(CRYPT_OBJS)          \
			   $(RBTREE_OBJS)         \
			   $(NETWORK_OBJS)

LIBS         = -lpthread -llua -ldl -lm

MAIN_LIBS    = $(LIBS) -llua -ldl -lm

CC           = gcc
CFLAGS       = -g -W -Wall -Werror -Wno-unused-parameter -Wunused-function \
			   -Wunused-variable -Wunused-value -fPIC \
			   -Ithirdparty/hashmap -Ithirdparty/conhash -Iinclude -Ithirdparty/rbtree

STATIC_LIB = libeasy.a

lib:$(OBJS)
	ar rcs $(STATIC_LIB) $(OBJS)

$(OBJS_DIR)/eshm.o:src/core/eshm.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/eshm.o src/core/eshm.c

$(OBJS_DIR)/ebuffer.o:src/core/ebuffer.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/ebuffer.o src/core/ebuffer.c

$(OBJS_DIR)/elog.o:src/core/elog.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/elog.o src/core/elog.c

$(OBJS_DIR)/etime.o:src/core/etime.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/etime.o src/core/etime.c

$(OBJS_DIR)/estring.o:src/core/estring.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/estring.o src/core/estring.c

$(OBJS_DIR)/eother.o:src/core/eother.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/eother.o src/core/eother.c

$(OBJS_DIR)/equeue.o:src/core/equeue.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/equeue.o src/core/equeue.c

$(OBJS_DIR)/emem_pool.o:src/core/emem_pool.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/emem_pool.o src/core/emem_pool.c

$(OBJS_DIR)/ethread_pool.o:src/core/ethread_pool.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/ethread_pool.o src/core/ethread_pool.c

$(OBJS_DIR)/ememory.o:src/core/ememory.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/ememory.o src/core/ememory.c

$(OBJS_DIR)/eprocess_lock.o:src/core/eprocess_lock.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/eprocess_lock.o  src/core/eprocess_lock.c

$(OBJS_DIR)/esyslog.o:src/core/esyslog.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/esyslog.o  src/core/esyslog.c

$(OBJS_DIR)/eproperty.o:src/core/eproperty.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/eproperty.o  src/core/eproperty.c

$(OBJS_DIR)/enetwork.o:src/core/enetwork.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/enetwork.o  src/core/enetwork.c

$(OBJS_DIR)/hashmap.o:thirdparty/hashmap/hashmap.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/hashmap.o  thirdparty/hashmap/hashmap.c

$(OBJS_DIR)/hash_fcn.o:thirdparty/hashmap/hash_fcn.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/hash_fcn.o  thirdparty/hashmap/hash_fcn.c

$(OBJS_DIR)/prime_table.o:thirdparty/hashmap/prime_table.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/prime_table.o   thirdparty/hashmap/prime_table.c

$(OBJS_DIR)/conhash.o:thirdparty/conhash/conhash.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/conhash.o   thirdparty/conhash/conhash.c

$(OBJS_DIR)/conhash_inter.o:thirdparty/conhash/conhash_inter.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/conhash_inter.o   thirdparty/conhash/conhash_inter.c

$(OBJS_DIR)/conhash_util.o:thirdparty/conhash/conhash_util.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/conhash_util.o   thirdparty/conhash/conhash_util.c

$(OBJS_DIR)/crc32.o:thirdparty/conhash/crc32.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/crc32.o   thirdparty/conhash/crc32.c

$(OBJS_DIR)/util_rbtree.o:thirdparty/conhash/util_rbtree.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/util_rbtree.o   thirdparty/conhash/util_rbtree.c

$(OBJS_DIR)/md5.o:thirdparty/crypt/md5.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/md5.o   thirdparty/crypt/md5.c

$(OBJS_DIR)/3des.o:thirdparty/crypt/3des.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/3des.o   thirdparty/crypt/3des.c

$(OBJS_DIR)/ip_zone.o:src/ip_zone/ip_zone.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/ip_zone.o src/ip_zone/ip_zone.c

$(OBJS_DIR)/server_conhash.o:src/server_conhash/server_conhash.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/server_conhash.o src/server_conhash/server_conhash.c

$(OBJS_DIR)/rbtree.o:thirdparty/rbtree/rbtree.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/rbtree.o thirdparty/rbtree/rbtree.c

$(OBJS_DIR)/udp.o:src/network/udp.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/udp.o src/network/udp.c

$(OBJS_DIR)/network_util.o:src/network/network_util.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/network_util.o src/network/network_util.c

$(OBJS_DIR)/tcp_server.o:src/network/tcp_server.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/tcp_server.o src/network/tcp_server.c

$(OBJS_DIR)/tcp_client.o:src/network/tcp_client.c
	$(CC) -c $(CFLAGS) -o $(OBJS_DIR)/tcp_client.o src/network/tcp_client.c

clean:
	rm -f $(OBJS) libeasy.a

install:
	mkdir -p /usr/local/include/easy
	install -m644 libeasy.a         /usr/local/lib	
	install -m644 include/*.h        /usr/local/include/easy
	install -m644 thirdparty/hashmap/*.h  /usr/local/include/easy
	install -m644 thirdparty/conhash/*.h  /usr/local/include/easy
	install -m644 src/ip_zone/ip_zone.h  /usr/local/include/easy
	install -m644 src/server_conhash/server_conhash.h  /usr/local/include/easy
	install -m644 thirdparty/crypt/*.h  /usr/local/include/easy
	install -m644 thirdparty/rbtree/rbtree.h  /usr/local/include/easy
	install -m644 src/network/*.h  /usr/local/include/easy

uninstall:
	rm -f  /usr/local/lib/libeasy.a
	rm -rf /usr/local/include/easy
