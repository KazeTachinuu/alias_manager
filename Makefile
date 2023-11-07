CC=gcc

SRC = aliasmanager.o
BIN = aliasmanager

all: ${BIN}

BIN: ${SRC}

clean:
	${RM} *.o ${BIN}

.PHONY: all, clean
