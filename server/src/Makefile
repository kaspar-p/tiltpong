CC = gcc
FLAGS = -Wall -Wpedantic

HEADERS = ../third_party/libcoap/include ../third_party/libwebsockets/include
LIBS = ../third_party/libcoap/.libs

all: main

clean:
	rm -rf main websocket *.o

websocket: websocket.o wsserver.o
	${CC} -lssl -lcrypto -L../third_party/libwebsockets/build/lib -lwebsockets $^ -o $@

websocket.o: websocket.c
	${CC} -I${HEADERS} ${FLAGS} -c $^ -o $@

wsserver.o: wsserver.c
	${CC} -I${HEADERS} -Iws.h ${FLAGS} -c $^ -o $@

main: main.o
	${CC} -lgnutls -L${LIBS} -lcoap-3-gnutls $^ -o $@

main.o: main.c
	${CC} -I${HEADERS} ${FLAGS} -c $^ -o $@
