#include "ng_readfile.h"

void showInputConf(struct host_instance* hosts, int num) {
	printf("virtual host num = %d\n", num);
	for (int i = 0; i < num; ++i) {
		printf("server %d {\n", i);
		printf("port: %d\n", hosts[i].port);
		printf("root: %s\n", hosts[i].root);
		printf("index: %s\n", hosts[i].index);
		printf("server_name: %s\n", hosts[i].server_name);
		printf("ssl: %d\n", hosts[i].ssl);
		printf("ssl_certificate: %s\n", hosts[i].ssl_certificate);
		printf("ssl_certificate_key: %s\n", hosts[i].ssl_certificate_key);
		printf("}\n");
	}
}

void getHostConf(FILE* fp, struct host_instance* host, char c[1000]) {
	char nc[MAX_LINE_CHARS];
	// Read port
	if (!strcmp(c, "listen")) {
		fscanf(fp, "%s;", nc);
		host->port = atoi(nc);
		// printf("listen: %d\n", host->port);
		return;
	}
	// Read root
	if (!strcmp(c, "root")) {
		fscanf(fp, "%s;", nc);
		strcpy(host->root, nc);
		// printf("root: %s\n", host->root);
		return;
	}
	// Read index
	if (!strcmp(c, "index")) {
		fscanf(fp, "%s;", nc);
		strcpy(host->index, nc);
		// printf("index: %s\n", host->index);
		return;
	}
	// Read server_name
	if (!strcmp(c, "server_name")) {
		fscanf(fp, "%s;", nc);
		strcpy(host->server_name, nc);
		// printf("server_name: %s\n", host->server_name);
		return;
	}
	// Read ssl_certificate
	if (!strcmp(c, "ssl_certificate")) {
		fscanf(fp, "%s;", nc);
		strcpy(host->ssl_certificate, nc);
		// printf("ssl_certificate: %s\n", host->ssl_certificate);
		return;
	}
	// Read ssl_certificate_key
	if (!strcmp(c, "ssl_certificate_key")) {
		fscanf(fp, "%s;", nc);
		strcpy(host->ssl_certificate_key, nc);
		// printf("ssl_certificate_key: %s\n", host->ssl_certificate_key);
		return;
	}
}

int add_port(struct port_list* port_lists, int port, char server_name[MAX_DOMAIN_LENGTH], int* lists_num, int id) {
	struct port_list* last_port;
	struct host_list* last_host;
	if (port_lists->port == 0) {
		port_lists->port = port;
		struct host_list* new_host = (struct host_list*) calloc(1, sizeof(struct host_list));
		new_host->id = id;
		strcpy(new_host->server_name, server_name);
		new_host->next = NULL;
		port_lists->host_lists = new_host;
		*lists_num = 1;
		return 0;
	}

	while (port_lists) {
		if (port_lists->port == port) {
			struct host_list* host_lists = port_lists->host_lists;
			while (host_lists) {
				if (strcmp(server_name, host_lists->server_name) == 0) {
					return 1;
				}
				last_host = host_lists;
				host_lists = host_lists->next;
			}
			struct host_list* new_host = (struct host_list*) calloc(1, sizeof(struct host_list));
			new_host->id = id;
			strcpy(new_host->server_name, server_name);
			new_host->next = NULL;
			last_host->next = new_host;
			return 0;
		}
		last_port = port_lists;
		port_lists = port_lists->next;
	}
	
	struct port_list* new_port = (struct port_list*) calloc(1, sizeof(struct port_list));
	struct host_list* new_host = (struct host_list*) calloc(1, sizeof(struct host_list));
	new_host->id = id;
	strcpy(new_host->server_name, server_name);
	new_host->next = NULL;
	new_port->port = port;
	new_port->host_lists = new_host;
	new_port->next = NULL;
	last_port->next = new_port;
	*lists_num += 1;
	return 0;
}

struct niginxxx_instance* ng_init_instance() {
	struct niginxxx_instance* niginxxx = (struct niginxxx_instance*) calloc(1, sizeof(struct niginxxx_instance));
	struct port_list* port_lists = (struct port_list*) calloc(1, sizeof(struct port_list));
	port_lists->port = 0;
	int lists_num = 0;
	int max_connections = 1024;
	int keepalive_timeout = 120;

	char c[MAX_LINE_CHARS];
	FILE* fp = NULL;
	fp = fopen("conf/default.conf", "r");

	if (fp == NULL) {
		printf("Error! opening file");
		return NULL;       
	}

	// Read max_connections
	fscanf(fp, getValue(max_connections), c);
	if (atoi(c) != 0) {
		max_connections = atoi(c);
	}
	niginxxx->max_connections = max_connections;
	printf("max_connections: %d\n", max_connections);

	// Read keepalive_timeout
	fscanf(fp, getValue(keepalive_timeout), c);
	if (atoi(c) != 0) {
		keepalive_timeout = atoi(c);
	}
	niginxxx->keepalive_timeout = keepalive_timeout;
	printf("keepalive_timeout: %d\n", keepalive_timeout);

	// Initialize the default virtual host
	struct host_instance* hosts = (struct host_instance*) calloc(1, sizeof(struct host_instance));
	hosts->port = 80;
	strcpy(hosts->root, "./www/default");
	strcpy(hosts->index, "index.html");
	strcpy(hosts->server_name, "_");

	// Get all virtual host
	int i = 1;
	while(!feof(fp)) {
		fscanf(fp, "%s", c);
		// Determine if there is still a virtual host
		if (!strcmp(c, "server")) {
			fscanf(fp, "%s", c);
			if (!strcmp(c, "{")) {
				// printf("server %d {\n", i);
				// Create a new host instance
				struct host_instance* n_host = (struct host_instance*) calloc(1, sizeof(struct host_instance));
				n_host->id = i-1;
				// Read the virtual host configuration file
				fscanf(fp, "%s", c);
				while (strcmp(c, "}") && !feof(fp)) {
					getHostConf(fp, n_host, c);
					c[0] = '\0';
					fscanf(fp, "%s", c);
				}
				if (n_host->root[0] == '\0') strcpy(n_host->root, "./www/default");
				if (n_host->index[0] == '\0') strcpy(n_host->index, "index.html");
				if (n_host->server_name[0] == '\0') strcpy(n_host->server_name, "_");
				if (n_host->ssl_certificate[0] != '\0' && n_host->ssl_certificate_key[0] != '\0') n_host->ssl = 1;
				// printf("}\n");
				if (add_port(port_lists, n_host->port, n_host->server_name, &lists_num, i-1)) printf("Duplicate server name and port\n");
				// hosts list expansion
				hosts = (struct host_instance*) realloc(hosts, i*sizeof(struct host_instance));
				hosts[i-1] = *n_host;
				i++;
			}
		}
	}

	fclose(fp);

	niginxxx->hosts = hosts;
	niginxxx->hosts_num = i==1 ? i : i-1;
	niginxxx->lists_num = lists_num;
	niginxxx->port_lists = port_lists;
	// printf("hosts_num = %d\n", niginxxx->hosts_num);
	// showInputConf(niginxxx->hosts, niginxxx->hosts_num);
	return niginxxx;
}

void ng_destroy_instance(struct niginxxx_instance* niginxxx) {
	free(niginxxx->hosts);
	struct port_list* port_list = niginxxx->port_lists;
	while(port_list) {
		struct port_list* fr_port_list = port_list;
		struct host_list* host_list = port_list->host_lists;
		port_list = port_list->next;
		while(host_list) {
			struct host_list* fr_host_list = host_list;
			host_list = host_list->next;
			free(fr_host_list);
		}
		free(fr_port_list);
	}
	free(niginxxx);
}
