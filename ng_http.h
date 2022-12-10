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
#define MAX_TCP_PACKAGE_SIZE 1024

#define status_codes_100(protocol) #protocol " 100 Continue\r\n"
#define status_codes_200(protocol) #protocol " 200 OK\r\n"
#define status_codes_201(protocol) #protocol " 201 Created\r\n"
#define status_codes_202(protocol) #protocol " 202 Accepted\r\n"
#define status_codes_204(protocol) #protocol " 204 No Content\r\n"
#define status_codes_206(protocol) #protocol " 206 Partial Content\r\n"
#define status_codes_301(protocol) #protocol " 301 Moved Permanently\r\n"
#define status_codes_302(protocol) #protocol " 302 Found\r\n"
#define status_codes_303(protocol) #protocol " 303 See Other\r\n"
#define status_codes_304(protocol) #protocol " 304 Not Modified\r\n"
#define status_codes_307(protocol) #protocol " 307 Temporary Redirect\r\n"
#define status_codes_308(protocol) #protocol " 308 Permanent Redirect\r\n"
#define status_codes_400(protocol) #protocol " 400 BAD REQUEST\r\n"
#define status_codes_401(protocol) #protocol " 401 Unauthorized\r\n"
#define status_codes_403(protocol) #protocol " 403 Forbidden\r\n"
#define status_codes_404(protocol) #protocol " 404 NOT FOUND\r\n"
#define status_codes_406(protocol) #protocol " 406 Not Acceptable\r\n"
#define status_codes_407(protocol) #protocol " 407 Proxy Authentication Required\r\n"
#define status_codes_409(protocol) #protocol " 409 Conflict\r\n"
#define status_codes_410(protocol) #protocol " 410 Gone\r\n"
#define status_codes_412(protocol) #protocol " 412 Precondition Failed\r\n"
#define status_codes_416(protocol) #protocol " 416 Range Not Satisfiable\r\n"
#define status_codes_418(protocol) #protocol " 418 I'm a teapot\r\n"
#define status_codes_451(protocol) #protocol " 451 Unavailable For Legal Reasons\r\n"
#define status_codes_500(protocol) #protocol " 500 Internal Sever Error\r\n"
#define status_codes_501(protocol) #protocol " 501 Method Not Implemented\r\n"
#define status_codes_502(protocol) #protocol " 502 Bad Gateway\r\n"
#define status_codes_503(protocol) #protocol " 503 Service Unavailable\r\n"
#define status_codes_504(protocol) #protocol " 504 Gateway Timeout\r\n"

/* forward declare */
struct host_list;
struct host_instance;
struct niginxxx_instance;

int get_require_line(int client_fd, char * buf, int size);
void handle_request(int client_fd, struct host_instance* hosts, struct host_list* host_lists);
int handle_response(int client_fd, int code, char* path);

#endif
