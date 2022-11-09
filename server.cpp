/*
 * server.c
 * Zeb Carty & Michael McInerney
 *
 * Creates multithreaded server
 */

#include "include.h"
#include "decode.cpp"
#include "log.cpp"
#include <sys/time.h>
using namespace std;

int timeout_secs = 15;
int req_secs = 60;
int num_reqs = 3;

// ./QRServer [option1, ... , optionN]
//  PORT [port number] (default: 2012)
//  RATE [number requests] [number seconds] (default: 3 requests/60 seconds)
//  MAX_USERS [number of users] (default: 3 users)
//  TIME_OUT [number of seconds] (default: 80 seconds)

bool *returned;

struct handler
{
	int file_descriptor;
	int thread_id;
	struct sockaddr_in client_address;
};

void *concurrency(void *inp)
{
	handler *input = (handler *)inp;
	int clientfd = input->file_descriptor;
	string client_ip = inet_ntoa(input->client_address.sin_addr);

	double *requests = (double *)malloc(sizeof(double) * num_reqs);
	int curr_reqs = 0;

	/* client-server interaction */
	unsigned filesize_buff[1];

	/* TIMEOUT */
	fd_set readfds;
	fd_set writefds;

	struct timeval tv;
	tv.tv_sec = timeout_secs;
	tv.tv_usec = 0;
 
	while (true)
	{
		/* Sets fds to zero */
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(clientfd, &readfds);
		//FD_SET(clientfd, &writefds);
		
		/* select will only accept a connection if a client connects before timeout */
		int r = select(FD_SETSIZE, &readfds, &writefds, NULL, &tv);
		cout << "select returned" << endl;
		cout << r << endl;
		
		if(r > 0) {
			read(clientfd, filesize_buff, sizeof(filesize_buff));

			cout << "From Client: " << endl;
			cout << "File size: " << filesize_buff[0] << " bytes" << endl;

			char buff[filesize_buff[0]];
			int bytes_read;

			bzero(buff, sizeof(buff));
			bytes_read = read(clientfd, buff, sizeof(buff));

			// ratelimiting functionality
			struct timeval t;
			gettimeofday(&t, NULL);
			double dtime = t.tv_usec;
			dtime = dtime / 1000000;
			dtime += t.tv_sec;

			requests[(curr_reqs % num_reqs)] = dtime;
			curr_reqs++;

			unsigned int three = 3;
			if (curr_reqs > num_reqs)
			{
				if ((requests[(curr_reqs % num_reqs)] - requests[((curr_reqs - 1) % num_reqs)]) < req_secs)
				{
					cout << "Error: ratelimit has been reached." << endl; // debug line
					log(client_ip, "Ratelimit has been reached");
					write(clientfd, &three, sizeof(unsigned));
					write(clientfd, "Error: ratelimit has been reached.\n", sizeof("Error: ratelimit has been reached.\n"));
					continue;
				}
			}

			// Image is in buffer now
			char filename[128];
			sprintf(filename, "picture%d.png", (input->thread_id));
			cout << "Created " << filename << endl;

			ofstream picture(filename);
			picture.write(buff, bytes_read);
			picture.close();

			string url = getURL(filename);

			bzero(buff, sizeof(buff));
			const char *url_char = url.c_str();
			strcpy(buff, url_char);

			unsigned urlsize_buff[1];
			urlsize_buff[0] = strlen(url_char);
			printf("urlsize: %d", urlsize_buff[0]);
			cout << endl;

			// copy server message in the buffer
			write(clientfd, urlsize_buff, sizeof(urlsize_buff)); // URL Size
			write(clientfd, buff, sizeof(buff));				 // URL

			char success_log[64];
			sprintf(success_log, "Server decoded with url: %s", buff);
			log(client_ip, success_log);
		} else {
			cout << "Time out" << endl;
			log(client_ip, "Time out");
			unsigned int two = 2;
			write(clientfd, &two, sizeof(unsigned));
			write(clientfd, "Error: Timeout.\n", sizeof("Error: Timeout.\n"));
			break;
		}
	}

	/* close the client socket */
	free(requests);
	close(clientfd);

	returned[input->thread_id] = true;
	return NULL;
}

int main(int argc, char **argv)
{
	//const char *ip_address = "10.63.4.1";
	const char *ip_address = "127.0.0.1";
	char *port_num = "2012";
	int max_users = 3;

	// concurrency variables
	handler *handlers;
	pthread_t *threads;
	bool *actives;
	int num_actives = 0;
	int tracker = 0;

	// Assign command line arguments
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "PORT") == 0)
		{
			if (2000 > atoi(argv[i + 1]) || atoi(argv[i + 1]) > 3000)
			{
				printf("Please designate a port between 2000 - 3000. Default port (2012) used.");
			}
			else
			{
				port_num = argv[i + 1];
			}
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
	struct sockaddr_in client_address;
	socklen_t client_len;

	// allocate memory for threads and status tracking
	threads = (pthread_t *)malloc(max_users * sizeof(pthread_t));
	handlers = (handler *)malloc(max_users * sizeof(handler));
	actives = (bool *)calloc(max_users, sizeof(bool));
	returned = (bool *)calloc(max_users, sizeof(bool));

	/* configure the host */
	cout << "Configuring host...";
	memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
	hints.ai_family = AF_INET;					/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;			/* TCP, streaming */
												/* connection with CS3516_team4_host1 on port_num */
	// r = getaddrinfo(ip_address, port_num, &hints, &server);
// 	if (r != 0)
// 	{
// 		cout << "failed" << endl;
// 		exit(1);
// 	}
// 	cout << "done" << endl;

	/* create the socket */
	cout << "Assign a socket...";
	// sockfd = socket(
// 		server->ai_family,	 /* domain, TCP here */
// 		server->ai_socktype, /* type, stream */
// 		server->ai_protocol	 /* protocol, IP */
// 	);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( atoi(port_num) );

	sockfd = socket(
		AF_INET,	 /* domain, TCP here */
		SOCK_STREAM, /* type, stream */
		0	 /* protocol, IP */
	);

	if (sockfd == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	const int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

	// struct timeval timeout;
	// timeout.tv_sec = timeout_secs;
	// timeout.tv_usec = 0;
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	/* bind - name the socket */
	cout << "Binding socket...";
	// r = bind(sockfd,
// 			 server->ai_addr,
// 			 server->ai_addrlen);
    r = bind(sockfd,
			 (struct sockaddr*)&address,
             sizeof(address));
	if (r == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;
	log(ip_address, "Server Started");

	/* listen for incoming connections */
	cout << "Listening...";
	r = listen(sockfd, num_reqs);
	if (r == -1)
	{
		cout << "failed" << endl;
		exit(1);
	}
	cout << "done" << endl;

	while (true)
	{
		/* accept a new connection */
		cout << "Accepting new connection " << endl;
		client_len = sizeof(client_address);
		clientfd = accept(sockfd,
							(struct sockaddr *)&client_address,
							&client_len);
		if (clientfd == -1)
		{
			cout << "failed" << endl;
			exit(1);
		}
		cout << "on file descriptor " << clientfd << endl;
		/* get client ip */
		string client_ip = inet_ntoa(client_address.sin_addr);
		log(client_ip, "Client Connected");

		for (int i = 0; i < max_users; i++)
		{
			if (returned[i] == true)
			{
				returned[i] = false;
				actives[i] = false;
				pthread_join(threads[i], NULL);
				num_actives--;
			}
		}
		if (num_actives >= max_users)
		{ // thread cleanup
			char error[] = "Error: max number of concurrent connections reached";
			log(client_ip, "Error: max number of concurrent connections reached");
			write(clientfd, error, sizeof(error));
			close(clientfd);
			continue;
		}

		// scan through actives to see what may not be used
		while (actives[tracker] == true)
		{
			tracker++;
			if (tracker >= max_users)
			{
				tracker = 0;
			}
		}

		// assign new thread to inactive id
		handlers[tracker].client_address = client_address;
		handlers[tracker].file_descriptor = clientfd;
		handlers[tracker].thread_id = tracker;
		actives[tracker] = true;
		num_actives++;

		// create the thread
		pthread_create(&threads[tracker], NULL, concurrency, &handlers[tracker]);
		log(client_ip, "Client Disconnected");
	}

	/* free allocated memory */
// 	freeaddrinfo(server);
	/* close the socket */
	close(sockfd);
	cout << "Socket closed, done" << endl;
	log(ip_address, "Server Closed");

	return 0;
}
