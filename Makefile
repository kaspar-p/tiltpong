all: server game

server:
	cd server && make;

game:
	cd game && make;

clean:
	cd server && make clean;
	cd game && make clean;
	cd client && make clean;

.PHONY: all server game clean
