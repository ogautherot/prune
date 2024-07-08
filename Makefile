
OS := $(shell uname -o)
EXT = 
ifeq ($(OS), Msys)
	EXT=.exe
endif

OBJS=obj/main.o
TARGET=prune$(EXT)

CFLAGS = -Wall -Wextra -g
LDFLAGS = ${CFLAGS}

.PHONY: all
all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJS} -lssl -lcrypto

obj/%.o: src/%.c
	mkdir -p obj
	${CC} -c ${CFLAGS} -o $@ $<

.PHONY: clean
clean:
	rm -f obj/* ${TARGET}
