/* connect with a TCP server */
/* this is the TCP client code */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fstream>
using namespace std;

#define MAX 64

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
    //printf("%s", server->ai_addr);

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

    
    printf("Enter the file path: ");
    string filename;
    cin >> filename;

    ifstream fin(filename, ios::in | ios::binary);
    int buff_size = fin.tellg();
    
    char buff[buff_size];
    bzero(buff, sizeof(buff));
    fin.read(buff, sizeof(buff));
    fin.close();

    write(sockfd, buff, sizeof(buff));
    bzero(buff, buff_size);
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);

    /* free allocated memory */
    freeaddrinfo(server);
    /* close the socket */
    close(sockfd);
    puts("Socket closed, done");
    return (0);
}
