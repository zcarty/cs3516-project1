/*
 * client.cpp
 * Zeb Carty & Michael McInerney
 *
 * Creates client to interact with server
 */
#include "include.h"
#include "log.cpp"

int main(int argc, char **argv)
{
    char *ip_address = "10.63.4.1";
    char *port_num = "2012";
    struct addrinfo hints, *server;
    int r, sockfd;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "PORT") == 0)
        {
            if (2000 > atoi(argv[i+1]) || atoi(argv[i+1]) > 3000)
            {
                printf("Please designate a port between 2000 - 3000. Default port (2012) used.");
            }
            else
            {
                port_num = argv[i + 1];
            }
        }
    }

    /* CONFIGURE */
    printf("Configuring host...");
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 connection */
    hints.ai_socktype = SOCK_STREAM; /* TCP, streaming */
    /* connection with localhost (zero) on port 2012 (CHANGE IP AND PORT NUM LATER) */
    r = getaddrinfo(ip_address, port_num, &hints, &server);
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

    bool finished = false;
    while(finished == false) {
        /* PROMPT USER FOR FILE PATH */
        printf("Enter the file path: ");
        string filename;
        cin >> filename;

        /* OPEN FILE */
        ifstream fin(filename, ios::in | ios::binary);
        int begin = fin.tellg();
        fin.seekg(0, ios::end);
        int end = fin.tellg();

        unsigned buff_size = end - begin; // Get file size, which will be max buffer size
        unsigned filesize_buff[1] = {buff_size};

        if(filesize_buff[0] > 4e6) {
            cout << "Return Code: 1\n Please designate a file under 4MB" << endl;
            continue;
        }

        char buff[buff_size];
        bzero(buff, sizeof(buff));
        fin.seekg(0);
        fin.read(buff, sizeof(buff)); // Put file contents into buffer
        fin.close();

        /* WRITE DATA TO SERVER */
        write(sockfd, filesize_buff, sizeof(filesize_buff)); // File size
        write(sockfd, buff, sizeof(buff));                   // Image
        bzero(buff, buff_size);

        /* READ DATA FROM SERVER */

        // URL SIZE
        unsigned urlsize_buff[1];
        read(sockfd, urlsize_buff, sizeof(urlsize_buff));

        // URL
        read(sockfd, buff, sizeof(buff));

        // RETURN CODE
        printf("From Server:\n");
        unsigned return_code = 0;
        if (urlsize_buff[0] == 0)
        {
            return_code = 1;
            printf("Return Code: %d\n", return_code);
            printf("Error: %s\n", buff);
        }
        else if (urlsize_buff[0] == 3)
        {
            return_code = 3;
            printf("Return Code: %d", return_code);
            printf("%s", buff);
        }
        else
        {
            printf("Return Code: %d\n", return_code);
            printf("URL Size: %d\n", urlsize_buff[0]);
            printf("URL: %s\n", buff);
        }

        char returncode_log[64];
        sprintf(returncode_log, "Server decoded with return code: %d", return_code);
        log(ip_address, returncode_log);

        // allow client to make multiple uploads
        printf("Would you like to upload another QR code? ");
        string option;
        cin >> option;

        if(option == "Y" || option == "y") finished = false;
        else finished = true;
    }
    /* FREE MEMORY */
    freeaddrinfo(server);

    /* CLOSE SOCKET */
    close(sockfd);
    puts("Socket closed, done");
    return 0;
}
