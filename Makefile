
OS := $(shell uname -o)
EXT = 
ifeq ($(OS), Msys)
	EXT=.exe
endif

OBJS=obj/main.o
TARGET=prune$(EXT)

.PHONY: all
all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${OBJS}

obj/%.o: src/%.c
	mkdir -p obj
	${CC} -c -o $@ $<

