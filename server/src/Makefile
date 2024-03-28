CC = clang
FLAGS = -Wall -O3 -Wpedantic -I../third_party/libcoap/include -I../third_party/libwebsockets/include -I/usr/local/include -I/usr/local/Cellar/gnutls/3.8.4

LIBS = -L../third_party/libcoap/.libs -lgnutls -lcoap-3-gnutls -L../third_party/libwebsockets/build/lib -lwebsockets

all: tiltpongsrv

clean:
	rm -rf main game websocket *.o

tiltpongsrv: main.o coap.o game.o websocket.o ws_callbacks.o utils.o
	${CC} -o $@ $^ ${FLAGS} ${LIBS}

main.o: main.c
	${CC} ${FLAGS} -c $^

websocket.o: websocket.c
	${CC} ${FLAGS} -c $^

ws_callbacks.o: ws_callbacks.c
	${CC} ${FLAGS} -c $^

utils.o: utils.c
	${CC} ${FLAGS} -c $^

game.o: game.c
	${CC} ${FLAGS} -c $^

coap.o: coap.c
	${CC} ${FLAGS} -c $^
