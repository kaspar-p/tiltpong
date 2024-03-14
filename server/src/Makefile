CC = gcc
FLAGS = -Wall -Wpedantic

HEADERS = ../third_party/libcoap/include
LIBS = ../third_party/libcoap/.libs

all: main

clean:
	rm -rf main.o main

main: main.o
	${CC} -lgnutls -I${HEADERS} ${LIBS}/libcoap-3-gnutls.a $^ -o $@

main.o: main.c
	${CC} -I${HEADERS} ${FLAGS} -c $^ -o $@
