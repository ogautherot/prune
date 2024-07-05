
OS := $(shell uname -o)
EXT = 
ifeq ($(OS), Msys)
	EXT=.exe
endif

OBJS=obj/main.o
TARGET=prune$(EXT)

CFLAGS = -Wall -Wextra 

.PHONY: all
all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${OBJS} -lssl -lcrypto

obj/%.o: src/%.c
	mkdir -p obj
	${CC} -c ${CFLAGS} -o $@ $<

.PHONY: clean
clean:
	rm -f obj/* ${TARGET}
