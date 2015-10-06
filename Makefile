all:
	gcc -fPIC -shared -o libnss_rotor.so.2 -Wl,-soname,libnss_rotor.so.2 src/udp_client.c src/rotd.c
install:
	sudo install -m 0644 libnss_rotor.so.2 /lib
	sudo /sbin/ldconfig /lib
clean:
	rm libnss_rotor.so.2
