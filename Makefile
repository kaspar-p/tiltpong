all:
	cd server && make;

clean:
	cd server && make clean;
	cd client && make clean;

.PHONY: all clean
