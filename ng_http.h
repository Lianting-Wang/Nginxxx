#ifndef NG_HTTP_H
#define NG_HTTP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "ng_readfile.h"

#define MAX_URL_LENGTH 2083
#define MAX_DOMAIN_LENGTH 253

#define status_codes_200(protocol) #protocol " 200 OK\r\n"
#define status_codes_201(protocol) #protocol " 200 CREATED\r\n"
#define status_codes_400(protocol) #protocol " 400 BAD REQUEST\r\n"
#define status_codes_404(protocol) #protocol " 404 NOT FOUND\r\n"
#define status_codes_500(protocol) #protocol " 500 Internal Sever Error\r\n"
#define status_codes_501(protocol) #protocol " 501 Method Not Implemented\r\n"

/* forward declare */
struct host_list;
struct host_instance;
struct niginxxx_instance;

int get_require_line(int client_fd, char * buf, int size);
void handle_request(int client_fd, struct host_instance* hosts, struct host_list* host_lists);
int handle_response(int client_fd, int code, char* path);

#endif
