#Makefile

TARGET = chat_client
CC = gcc
OPTION = -g -DDBUG
LIB = -lpthread
SRC_FILES = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC_FILES))

%.o: %.c
	${CC} ${OPTION} -c $< 

${TARGET}: ${OBJS}
	${CC} ${OPTION} -o $@ $^ ${LIB} 
	
.PHONY: clean
clean:
	rm -rf ${OBJS} ${TARGET}