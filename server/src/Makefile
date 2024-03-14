CC = gcc
FLAGS = -Wall -Wpedantic

HEADERS = ../third_party/libcoap/include ../third_party/libwebsockets/include
LIBS = ../third_party/libcoap/.libs ../third_party/libwebsockets/build/lib

all: main websocket

clean:
	rm -rf main.o main websocket.o websocket

websocket: websocket.o
	${CC} -lssl -lcrypto ../third_party/libwebsockets/build/lib/libwebsockets.a $^ -o $@

websocket.o: websocket.c
	${CC} -I${HEADERS} ${FLAGS} -c $^ -o $@

main: main.o
	${CC} -lgnutls -L${LIBS} -lcoap-3-gnutls $^ -o $@

main.o: main.c
	${CC} -I${HEADERS} ${FLAGS} -c $^ -o $@
