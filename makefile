all: QRServer

server: quickserver.cpp
	g++ -Wall -pthread server.cpp -o QRServer
	./QRServer

client: client.cpp
	g++ -Wall client.cpp -o QRClient
	./QRClient

clean:
	rm -f QRServer
	rm -f QRClient
	rm -f a.out
	rm -f server
	rm -f client