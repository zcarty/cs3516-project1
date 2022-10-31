/*
* server.c
* Zeb Carty & Michael McInerney
* 
* Creates single-threaded server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

// ./QRServer [option1, ... , optionN]
//  PORT [port number] (default: 2012)
//  RATE [number requests] [number seconds] (default: 3 requests/60 seconds)
//  MAX USERS [number of users] (default: 3 users)
//  TIME OUT [number of seconds] (default: 80 seconds)

