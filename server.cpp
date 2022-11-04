/*
 * server.c
 * Zeb Carty & Michael McInerney
 *
 * Creates single-threaded server
 */

#include "include.h"
#include "decode.cpp"

// ./QRServer [option1, ... , optionN]
//  PORT [port number] (default: 2012)
//  RATE [number requests] [number seconds] (default: 3 requests/60 seconds)
//  MAX_USERS [number of users] (default: 3 users)
//  TIME_OUT [number of seconds] (default: 80 seconds)

int main(int argc, char **argv)
{
	char *port_num = "2012";
	int num_reqs = 3;
	int req_secs = 60;
	int max_users = 3;
	int timeout_secs = 80;

	/* ASSIGN COMMAND LINE ARGS */ 
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "PORT") == 0)
		{
			port_num = argv[i + 1];
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

	struct addrinfo hints, *server;
	int r, sockfd, clientfd;
	struct sockaddr client_address;
	socklen_t client_len;

	/* CONFIGURE */
	cout << "Configuring host...";
	memset(&hints, 0, sizeof(struct addrinfo)); 
	hints.ai_family = AF_INET;					/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;			/* TCP, streaming */
    /* connection with localhost (zero) on port 2012 (CHANGE IP AND PORT NUM LATER) */
	r = getaddrinfo(0, port_num, &hints, &server);
	if (r != 0)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

    /* CREATE SOCKET */
	cout << "Assign a socket...";
	sockfd = socket(
		server->ai_family,	 /* domain, TCP here */
		server->ai_socktype, /* type, stream */
		server->ai_protocol	 /* protocol, IP */
	);

	if (sockfd == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

    /* BIND SOCKET */
	cout << "Binding socket...";
	r = bind(sockfd,
			 server->ai_addr,
			 server->ai_addrlen);
	if (r == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	/* LISTEN */
	cout << "Listening...";
	r = listen(sockfd, num_reqs);
	if (r == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	/* ACCEPT */
	cout << "Accepting new connection ";
	client_len = sizeof(client_address);
	clientfd = accept(sockfd,
					  &client_address,
					  &client_len);
	if (clientfd == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "on file descriptor " << clientfd << endl;

	/* CLIENT-SERVER INTERACTION*/
	char buff[10000];

	bzero(buff, sizeof(buff));
	read(clientfd, buff, sizeof(buff));

	// Image is in buffer now
	ofstream picture("picture.png");
	picture.write(buff, sizeof(buff));
	picture.close();
	
	string url;
	
	url = getURL("picture.png");
	int url_size = url.length();
	char url_char[url_size + 1];
	strcpy(url_char, url.c_str());

	bzero(buff, sizeof(buff));

	for (int i = 0; i < url_size; i++)
	{
		buff[i] = url_char[i];
	}
	
	write(clientfd, buff, sizeof(buff));

	/* CLOSE SOCKET */
	close(clientfd);

    /* FREE MEMORY */
    freeaddrinfo(server);

    /* CLOSE SOCKET */
	close(sockfd);
	cout << "Socket closed, done";

	return 0;
}