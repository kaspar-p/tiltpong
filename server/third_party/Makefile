all: libcoap libwebsockets

libcoap: libcoap/.libs/libcoap-3-gnutls.a

libcoap/.libs/libcoap-3-gnutls.a:
	cd libcoap && \
	./autogen.sh && \
	./configure --disable-doxygen --disable-manpages && \
	$(MAKE) -j8 && \
	sudo $(MAKE) install

libwebsockets: libwebsockets/lib/libwebsockets.a

libwebsockets/lib/libwebsockets.a:
		cd libwebsockets && \
		cmake -S . -B build -DDISABLE_WERROR=ON && \
		cd build && \
		make -j8 && \
		sudo make install

clean:
	cd libcoap && make clean;
	cd libwebsockets && rm -rf build;

.PHONY: all clean libcoap libwebsockets
