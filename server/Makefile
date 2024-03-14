all: third_party src

src:
	cd src && make;

third_party:
	git submodule update --init && cd third_party && make;

clean:
	cd src && make clean;
	cd third_party && make clean;

.PHONY: all clean src third_party
