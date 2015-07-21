OBJS_DIR     = objs

SRCS         = $(wildcard *.c ./sub/*.c) 
EXES         = $(patsubst %.c,%,  $(SRCS))
			   

LIBS         = -leasy

CC           = gcc
CFLAGS       = -g -W -Wall -Wno-unused-parameter -Wunused-function \
			   -Wunused-variable -Wunused-value -Werror

default:$(EXES)

%:%.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

clean:
	rm -rf $(EXES)


