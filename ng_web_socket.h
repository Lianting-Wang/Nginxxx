#ifndef NG_WEB_SOCKET_H
#define NG_WEB_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#ifndef BACKLOG
#define BACKLOG 20		// Maximum number of requests allowed in the request queue
#endif

int listening_port(int port);
int accept_client(int sock_fd);
int close_socket(int sock_fd);

#endif
