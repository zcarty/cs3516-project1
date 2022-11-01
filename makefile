all: QRServer

server: server.cpp
	g++ -Wall server.cpp -o QRServer

clean:
	rm QRServer