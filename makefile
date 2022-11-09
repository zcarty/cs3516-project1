all: QRServer QRClient

server: server.cpp
	g++ -pthread server.cpp -o QRServer

quickserver: server.cpp
	g++ -Wall -pthread server.cpp -o QRServer
	./QRServer

quickclient: client.cpp
	g++ -Wall client.cpp -o QRClient
	./QRClient

client: client.cpp
	g++ -Wall client.cpp -o QRClient

clean:
	rm -f QRServer
	rm -f QRClient
	rm -f a.out
	rm -f server
	rm -f client