all: QRServer

QRServer: server.c
	gcc -Wall server.c -o QRServer

clean:
	rm QRServer