/*
 * server.c
 * Zeb Carty & Michael McInerney
 *
 * Creates single-threaded server
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

// ./QRServer [option1, ... , optionN]
//  PORT [port number] (default: 2012)
//  RATE [number requests] [number seconds] (default: 3 requests/60 seconds)
//  MAX_USERS [number of users] (default: 3 users)
//  TIME_OUT [number of seconds] (default: 80 seconds)

int main(int argc, char **argv)
{
    int port_num = 2012;
    int num_reqs = 3;
    int req_secs = 60;
    int max_users = 3;
    int timeout_secs = 80;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "PORT") == 0)
        {
            port_num = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "RATE") == 0)
        {
            num_reqs = stoi(argv[i + 1]);
            req_secs = stoi(argv[i + 2]);
        }
        else if (strcmp(argv[i], "MAX_USERS") == 0)
        {
            max_users = stoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "TIME_OUT") == 0)
        {
            timeout_secs = stoi(argv[i + 1]);
        }
    }
    cout << argc;
    cout << port_num;

    struct addrinfo hints, *server;
    int r, sockfd, clientfd;
    struct sockaddr client_address;
    socklen_t client_len;

    return 0;
}