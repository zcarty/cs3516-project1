/* connect with a TCP server */
/* this is the TCP client code */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

int main()
{
    struct addrinfo hints, *server;
    int r, sockfd;

    /* configure the host */
    printf("Configuring host...");
    memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
    hints.ai_family = AF_INET;                  /* IPv4 connection */
    hints.ai_socktype = SOCK_STREAM;            /* TCP, streaming */
    /* connection with localhost (zero) on port 8080 */
    r = getaddrinfo(0, "2012", &hints, &server);
    if (r != 0)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /* create the socket */
    printf("Assign a socket...");
    sockfd = socket(
        server->ai_family,   /* domain, TCP here */
        server->ai_socktype, /* type, stream */
        server->ai_protocol  /* protocol, IP */
    );
    if (sockfd == -1)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /* bind - name the socket */
    printf("Connecting socket...");
    r = connect(sockfd,
                server->ai_addr,
                server->ai_addrlen);
    if (r == -1)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /* Ask user for string, and create buffer */
    char *input;
    cin >> input;
    int input_len = strlen(input);

    char echoBuffer[32];
    int bytesRcvd;

    /* at this point you can interact with the TCP server */
    if (send(sockfd, input, input_len, 0))
    {
        perror("Failed to send");
    }

    int totalBytesRcvd = 0;
    printf("Recived:");
    while (totalBytesRcvd < input_len)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
                                     a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sockfd, echoBuffer, 32 - 1, 0)) <= 0)
            perror("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;  /* Keep tally of total bytes */
        echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer);     /* Print the echo buffer */
    }
    printf("\n");

    /* free allocated memory */
    freeaddrinfo(server);
    /* close the socket */
    close(sockfd);
    puts("Socket closed, done");
    return (0);
}
