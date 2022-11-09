Zeb Carty and Michael McInerney

Currently we have implemented a single-threaded server that can decode QR codes. Currently
we set up the server so that it must be run on the same machine. 

To use:
run 'make server' 
In a new terminal, run 'make client'
QRClient will prompt the user for a filepath (user must submit a png file of a QR Code)
The server will produce the the output from ZXing , including the url represented by that QR Code

Return Code Key (from resources):
0 - Success. The URL is being returned as specified below.
1 - Failure. Something went wrong and no URL is being returned. The character array length is set
to 0 and no character array is transmitted. This condition can be valid if the image uploaded does not
represent a valid QR code or the client violates our network security requirements.
2 - Timeout. The connection is being closed. A human-readable text message is created and supplied
as the character array. The character array length is set to the length of this human readable message.
3 - Rate Limit Exceeded. An error message about the rate limit being exceed is set in the character
array with the size set to the character array.


todo:
(TODO)-test all command args
        -port num
        -rate limiting
        -concurrent users (done?)
        -time out
            -disconnect clients if time out
(TODO)-rate limiting (by socket)
(DONE)-admin log
            -server start/end
            -user connected/disconnect
            -qr code decoded/error
(DONE)-client should send the server the file size (to designate buffer size)
(ZEB)-server sends back correct message
        -return code (2 and 3 needed)
        -url length (DONE)
        -url (DONE)
(DONE)-set image upper bound
(DONE)-send error messages back to client if file size issues
(ZEB) -readme