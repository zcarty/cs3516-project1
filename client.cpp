/*
 * client.cpp
 * Zeb Carty & Michael McInerney
 *
 * Creates client to interact with server
 */
#include "include.h"

int main()
{
    struct addrinfo hints, *server;
    int r, sockfd;

    /* CONFIGURE */
    printf("Configuring host...");
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 connection */
    hints.ai_socktype = SOCK_STREAM; /* TCP, streaming */
    /* connection with localhost (zero) on port 2012 (CHANGE IP AND PORT NUM LATER) */
    r = getaddrinfo(0, "2012", &hints, &server);
    if (r != 0)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /* CREATE SOCKET */
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

    /* BIND SOCKET */
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

    /* PROMPT USER FOR FILE PATH */
    printf("Enter the file path: ");
    string filename;
    cin >> filename;

    /* OPEN FILE */
    ifstream fin(filename, ios::in | ios::binary);
    int begin = fin.tellg();
    fin.seekg(0, ios::end);
    int end = fin.tellg();
    int buff_size = end - begin; // Get file size, which will be max buffer size
    printf("%d", buff_size);

    char buff[buff_size];
    bzero(buff, sizeof(buff));
    fin.seekg(0);
    fin.read(buff, sizeof(buff)); // Put file contents into buffer
    fin.close();

    /* WRITE DATA TO SERVER */
    write(sockfd, buff, sizeof(buff));
    bzero(buff, buff_size);
    read(sockfd, buff, sizeof(buff)); // Read data back from server
    printf("From Server : %s", buff);

    /* FREE MEMORY */
    freeaddrinfo(server);

    /* CLOSE SOCKET */
    close(sockfd);
    puts("Socket closed, done");
    return 0;
}
