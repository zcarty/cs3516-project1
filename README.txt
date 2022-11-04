Zeb Carty and Michael McInerney

Currently we have implemented a single-threaded server that can decode QR codes. Currently
we set up the server so that it must be run on the same machine. 

To use:
run 'make server' 
In a new terminal, run 'make client'
QRClient will prompt the user for a filepath (user must submit a png file of a QR Code)
The server will produce the the output from ZXing , including the url represented by that QR Code


todo:
-test all command args
    -port num
    -rate limiting
    -concurrent users (done?)
    -time out
        -disconnect clients if time out
-rate limiting (by socket)
-admin log
-client should send the server the file size (to designate buffer size)
-server sends back correct message
    -return code
    -url length
    -url
-set image upper bound
-send error messages back to client if file size issues
-readme