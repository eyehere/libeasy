all:libeasy.a
.PHONY:all

debug:libeasy.a
.PHONY:debug

libeasy.a:
	make -C src/lib/base

.PHONY:clean
clean:
	make -C src/lib/base clean