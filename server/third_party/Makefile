all: libcoap

libcoap: libcoap/.libs/libcoap-3-gnutls.a

libcoap/.libs/libcoap-3-gnutls.a:
	cd libcoap && \
	./autogen.sh && \
	./configure --disable-doxygen --disable-manpages && \
	$(MAKE)

clean:
	cd libcoap && make clean;

.PHONY: all clean libcoap
