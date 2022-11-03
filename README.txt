Zeb Carty and Michael McInerney

Currently we have implemented a single-threaded server that can decode QR codes. Currently
we set up the server so that it must be run on the same machine.

To use:
run 'make server' 
In a new terminal, run 'make client'
QRClient will prompt the user for a filepath (user must submit a png file of a QR Code)
The server will produce the url represented by that QR Code
