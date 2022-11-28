#ifndef NG_MAIN_H
#define NG_MAIN_H

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "ng_readfile.h"

/* forward declare */
struct port_list;
struct host_list;
struct host_instance;
struct niginxxx_instance;

/* -- ng_readfile.c -- */
struct niginxxx_instance* ng_init_instance();
void ng_destroy_instance(struct niginxxx_instance* niginxxx);

/* -- ng_web_socket.c -- */
#define BACKLOG 20    // Maximum number of requests allowed in the request queue

int listening_port(int port);
int accept_client(int sock_fd);
int close_socket(int sock_fd);

/* -- ng_http.c -- */
#ifndef NG_HTTP_H
void handle_request(int client_fd, struct host_instance* hosts, struct host_list* host_lists);
#endif

#endif
