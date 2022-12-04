#ifndef NG_METHOD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ng_http.h"

#define OPTIONS_C 1
#define GET_C 2
#define HEAD_C 3
#define POST_C 4
#define PUT_C 5
#define DELETE_C 6
#define TRACE_C 7
#define CONNECT_C 8 

#define OPTIONS "OPTIONS"
#define GET "GET"
#define HEAD "HEAD"
#define POST "POST"
#define PUT "PUT"
#define DELETE "DELETE"
#define TRACE "TRACE"
#define CONNECT "CONNECT"

int handle_post(int client_fd, char * path, int content_length);
char * handle_get(char * path);

#endif
