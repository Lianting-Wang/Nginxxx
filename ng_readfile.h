#ifndef NG_READFILE_H
#define NG_READFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ng_main.h"

#define MAX_VIRTUAL_HOST_NUM 100
#define MAX_LINE_CHARS 1000
#define MAX_URL_LENGTH 2083
#define MAX_DOMAIN_LENGTH 253

#define getValue(key) #key " %[^;] %*[^\n]%*c"

struct host_list {
	int id;
	char server_name[MAX_DOMAIN_LENGTH];
	struct host_list* next;
};

struct port_list {
	int  port;
	struct host_list* host_lists;
	struct port_list* next;
};

struct host_instance {
	int id;
	int  port;   /* Port that server need to listen */
	char root[MAX_URL_LENGTH];  /* WebApp locate */
	char index[MAX_URL_LENGTH];  /* Index file name */
	char server_name[MAX_DOMAIN_LENGTH];  /* Virtual host name */
	int ssl;  /* Determine if ssl protocol is used */
	char ssl_certificate[MAX_URL_LENGTH];  /* SSL certificate locate */
	char ssl_certificate_key[MAX_URL_LENGTH];  /* SSL certificate key locate */
};

struct niginxxx_instance {
	int max_connections;    /* Max connections */
	int keepalive_timeout;  /* Keepalive timeout */
	int hosts_num;  /* Host instance number */
	int lists_num;	/* Virtual host number */
	struct port_list* port_lists;	/* Virtual host ports list */
	struct host_instance* hosts;  /* Host instance list */
};

void showInputConf(struct host_instance* hosts, int num);
void getHostConf(FILE* fp, struct host_instance* host, char c[1000]);
int add_port(struct port_list* port_lists, int port, char server_name[MAX_DOMAIN_LENGTH], int* lists_num, int id);
struct niginxxx_instance* ng_init_instance();
void ng_destroy_instance(struct niginxxx_instance* niginxxx);

#endif
