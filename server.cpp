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

	// Assign command line arguments
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

	/* configure the host */
	cout << "Configuring host...";
	memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
	hints.ai_family = AF_INET;					/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;			/* TCP, streaming */
	/* connection with localhost (zero) on port number */
	r = getaddrinfo(0, port_num, &hints, &server);
	if (r != 0)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	/* create the socket */
	cout << "Assign a socket...";
	sockfd = socket(
		server->ai_family,	 /* domain, TCP here */
		server->ai_socktype, /* type, stream */
		server->ai_protocol	 /* protocol, IP */
	);
	// if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ))
	if (sockfd == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	/* bind - name the socket */
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

	/* listen for incoming connections */
	cout << "Listening...";
	r = listen(sockfd, num_reqs);
	if (r == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	/* accept a new connection */
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

	/* client-server interaction */
	char buff[10000];
	int n;

	bzero(buff, sizeof(buff));
	read(clientfd, buff, sizeof(buff));

	// Image is in buffer now
	ofstream picture("picture.png");
	picture.write(buff, sizeof(buff));
	picture.close();
	
	getURL("picture.png");

	// printf("From client: %s\t To client : ", buff);
	bzero(buff, sizeof(buff));

	// copy server message in the buffer
	buff[0] = 'h';
	write(clientfd, buff, sizeof(buff));

	/* close the client socket */
	close(clientfd);

	/* free allocated memory */
	freeaddrinfo(server);
	/* close the socket */
	close(sockfd);
	cout << "Socket closed, done";

	return 0;
}