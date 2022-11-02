all: QRServer

server: server.cpp
	g++ -Wall server.cpp -o QRServer
	./QRServer

client: client.cpp
	g++ -Wall client.cpp -o QRClient
	./QRClient

clean:
	rm QRServer