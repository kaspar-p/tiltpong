CC = gcc
FLAGS = -Wall -Wpedantic

LIBS = ../third_party/libcoap/.libs

all: main

clean:
	rm -rf main.o main

main: main.o
	${CC} -L${LIBS} -lcoap-3-gnutls $^ -o $@

main.o: main.c
	${CC} ${FLAGS} -c $^ -o $@
