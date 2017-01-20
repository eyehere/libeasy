all:libeasy.a
.PHONY:all

debug:libeasy.a
.PHONY:debug

libeasy.a:
	make -C src/

.PHONY:clean
clean:
	make -C src/ clean