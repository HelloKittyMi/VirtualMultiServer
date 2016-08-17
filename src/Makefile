DIR_INC = ./include
DIR_SRC = ./main
DIR_BIN = ../bin
DIR_OBJ = ../obj
SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))
BIN_TARGET = ${DIR_BIN}/${TARGET}

CC=gcc

TARGET+=
CFLAGS+=-g -Wall -Wno-unused  -I$(DIR_INC)
LDFLAGS= -L. -lpthread -lm
${BIN_TARGET}:${OBJ}
	$(CC) $(LDFLAGS) $(OBJ) $(CFLAGS) -o $@
#${BIN_TARGET}:${OBJ}
#	$(CC) $(LDFLAGS) $(CFLAGS) -o $(TARGET) $(OBJ) 

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(LDFLAGS) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -rf ${DIR_OBJ}/* 
