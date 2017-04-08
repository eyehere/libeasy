CC				= gcc
CFLAGS			= 
LIBS			= -lpthread
			  
SRCS			= $(wildcard *.c)
OBJS			= $(patsubst %.c,%.o,$(SRCS))

SRC_DIRS 		= $(dir $(SRCS))
SRC_PATH 		= $(shell echo $(SRC_DIRS) | tr -s " " "\n" | sort | uniq | tr -s "\n" ":")

VPATH 			= $(SRC_PATH)
INCLUDES		= $(patsubst %,-I%,$(subst :, ,$(VPATH)))

LIB_EASY		= libeasy.a

all:$(OBJS)
	ar rcs $(LIB_EASY) $(OBJS)
	@echo $(LIB_EASY) is generated
	-mkdir ../../../lib
	-cp $(LIB_EASY) ../../../lib/

$(OBJS):%.o:%.c
	$(CC) $(INCLUDES) -c -o $@ $<

.PHONY:clean
clean:
	-rm $(OBJS)
	-rm $(LIB_EASY)
	-rm -rf ../../../lib
