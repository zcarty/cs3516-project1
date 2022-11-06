/*
 * server.c
 * Zeb Carty & Michael McInerney
 *
 * Creates multithreaded server
 */

#include "include.h"
#include "decode.cpp"


// ./QRServer [option1, ... , optionN]
//  PORT [port number] (default: 2012)
//  RATE [number requests] [number seconds] (default: 3 requests/60 seconds)
//  MAX_USERS [number of users] (default: 3 users)
//  TIME_OUT [number of seconds] (default: 80 seconds)

bool* returned;

struct handler {
    int file_descriptor;
    int thread_id;
};

void* concurrency(void* inp) {
    handler* input = (handler*) inp;
    int clientfd = input -> file_descriptor;

	unsigned filesize_buff[1];
	read(clientfd, filesize_buff, sizeof(filesize_buff));
	// printf("filesize: %d", filesize_buff[0]);

	/* client-server interaction */
	char buff[filesize_buff[0]];
	int bytes_read;

	bzero(buff, sizeof(buff));
	bytes_read = read(clientfd, buff, sizeof(buff));

	// Image is in buffer now
    char filename[128];
    sprintf(filename, "picture%d.png", (input -> thread_id));

	ofstream picture(filename);
	picture.write(buff, bytes_read);
	picture.close();
	
	string url = getURL(filename);

	// printf("From client: %s\t To client : ", buff);
	bzero(buff, sizeof(buff));
	const char *url_char = url.c_str();
    strcpy(buff, url_char);

	unsigned urlsize_buff[1];
	urlsize_buff[0] = strlen(url_char);
	printf("urlsize: %d", urlsize_buff[0]);

	// copy server message in the buffer
	write(clientfd, urlsize_buff, sizeof(urlsize_buff)); // URL Size
	write(clientfd, buff, sizeof(buff));				 // URL

	/* close the client socket */
	close(clientfd);
    
    returned[input -> thread_id] = true;
	return NULL;
}

int main(int argc, char **argv)
{
	const char *ip_address = "10.63.4.1";
	char *port_num = "2012";
	int num_reqs = 3;
	int req_secs = 60;
	int max_users = 3;
	int timeout_secs = 80;

    // concurrency variables
    handler* handlers;
    pthread_t* threads;
    bool* actives;
    int num_actives = 0;
    int tracker = 0;

	// Assign command line arguments
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "PORT") == 0)
		{
			if(2000 > atoi(argv[i]) || atoi(argv[i]) > 3000)
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
	struct sockaddr client_address;
	socklen_t client_len;

    // allocate memory for threads and status tracking
    threads = (pthread_t*) malloc(max_users*sizeof(pthread_t));
    handlers = (handler*) malloc(max_users*sizeof(handler));
    actives = (bool*) calloc(max_users, sizeof(bool));
    returned = (bool*) calloc(max_users, sizeof(bool));

	/* configure the host */
	cout << "Configuring host...";
	memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
	hints.ai_family = AF_INET;					/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;			/* TCP, streaming */
	/* connection with localhost (zero) on port number */
	r = getaddrinfo(ip_address, port_num, &hints, &server);
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

    for(;;) {
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
        for(int i = 0; i < max_users; i++) {
            if(returned[i] == true) {
                returned[i] = false;
                actives[i] = false;
                pthread_join(threads[i], NULL);
                num_actives--;
            }
        } if(num_actives >= max_users) { // thread cleanup
            char error[] = "Error: max number of concurrent connections reached";
            write(clientfd, error, sizeof(error));
            close(clientfd);
            continue;
        }

        // scan through actives to see what may not be used
        while(actives[tracker] == true) {
            tracker++;
            if(tracker >= max_users) {
                tracker = 0;
            }
        }

        // assign new thread to inactive id
        handlers[tracker].file_descriptor = clientfd;
        handlers[tracker].thread_id = tracker;
        actives[tracker] = true; num_actives++;

        // create the thread
        pthread_create(&threads[tracker], NULL, concurrency, &handlers[tracker]);
        
    }

	/* free allocated memory */
	freeaddrinfo(server);
	/* close the socket */
	close(sockfd);
	cout << "Socket closed, done";

	return 0;
}