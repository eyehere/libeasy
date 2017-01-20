CC      = gcc
#LIB		= -leasy -luuid
LIB		= -leasy
CFLAGS  = -g -W -Wall -Wno-unused-parameter -Wunused-function \
		  -Wunused-variable -Wunused-value -Werror -fPIC
OBJS    = easy_lua.o 

lua:$(OBJS)
	$(CC) -shared -o easy_lua.so easy_lua.o $(LIB)

easy_lua.o:easy_lua.c
	$(CC) -c $(CFLAGS) -o easy_lua.o easy_lua.c

install:
	 install -m644 easy_lua.so  /opt/soft/base_env/openresty/lualib

clean:
	rm -f easy_lua.so 
	rm -f easy_lua.o
