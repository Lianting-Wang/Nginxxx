#ifndef NG_HTTP_H
#define NG_HTTP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "ng_readfile.h"
#include "ng_method.h"

#define MAX_URL_LENGTH 2083
#define MAX_DOMAIN_LENGTH 253
#define MAX_TCP_PACKAGE_SIZE 1024

#define status_codes_100(protocol) #protocol " 100 CONTINUE\r\n"
#define status_codes_200(protocol) #protocol " 200 OK\r\n"
#define status_codes_201(protocol) #protocol " 201 CREATED\r\n"
#define status_codes_202(protocol) #protocol " 202 ACCEPTED\r\n"
#define status_codes_204(protocol) #protocol " 204 NO CONTENT\r\n"
#define status_codes_206(protocol) #protocol " 206 PARTIAL CONTENT\r\n"
#define status_codes_301(protocol) #protocol " 301 MOVED PERMANENTLY\r\n"
#define status_codes_302(protocol) #protocol " 302 FOUND\r\n"
#define status_codes_303(protocol) #protocol " 303 SEE OTHER\r\n"
#define status_codes_304(protocol) #protocol " 304 NOT MODIFIED\r\n"
#define status_codes_307(protocol) #protocol " 307 TEMPORARY REDIRECT\r\n"
#define status_codes_308(protocol) #protocol " 308 PERMANENT REDIRECT\r\n"
#define status_codes_400(protocol) #protocol " 400 BAD REQUEST\r\n"
#define status_codes_401(protocol) #protocol " 401 UNAUTHORIZED\r\n"
#define status_codes_403(protocol) #protocol " 403 FORBIDDEN\r\n"
#define status_codes_404(protocol) #protocol " 404 NOT FOUND\r\n"
#define status_codes_405(protocol) #protocol " 405 METHOD NOT ALLOWED\r\n"
#define status_codes_406(protocol) #protocol " 406 NOT ACCEPTABLE\r\n"
#define status_codes_407(protocol) #protocol " 407 PROXY AUTHENTICATION REQUIRED\r\n"
#define status_codes_409(protocol) #protocol " 409 CONFLICT\r\n"
#define status_codes_410(protocol) #protocol " 410 GONE\r\n"
#define status_codes_411(protocol) #protocol " 411 LENGTH REQUIRED\r\n"
#define status_codes_412(protocol) #protocol " 412 PRECONDITION FAILED\r\n"
#define status_codes_416(protocol) #protocol " 416 RANGE NOT SATISFIABLE\r\n"
#define status_codes_418(protocol) #protocol " 418 I'M A TEAPOT\r\n"
#define status_codes_451(protocol) #protocol " 451 UNAVAILABLE FOR LEGAL REASONS\r\n"
#define status_codes_500(protocol) #protocol " 500 INTERNAL SERVER ERROR\r\n"
#define status_codes_501(protocol) #protocol " 501 METHOD NOT IMPLEMENTED\r\n"
#define status_codes_502(protocol) #protocol " 502 BAD GATEWAY\r\n"
#define status_codes_503(protocol) #protocol " 503 SERVICE UNAVAILABLE\r\n"
#define status_codes_504(protocol) #protocol " 504 GATEWAY TIMEOUT\r\n"

/* forward declare */
struct host_list;
struct host_instance;
struct niginxxx_instance;

int get_require_line(int client_fd, char * buf, int size);
void handle_request(int client_fd, struct host_instance* hosts, struct host_list* host_lists);
int handle_response(int client_fd, int code, char* path, char method[]);

#endif
