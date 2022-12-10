#include "ng_http.h"

struct stat st;

int get_require_line(int client_fd, char * buf, int size) {
	int num = 0;
	int len = 1;
	char c = '\0';
	while(len) {
		len = read(client_fd, &c, 1);
		if (c == '\r') continue;
		if (c == '\n') break;
		buf[num] = c;
		num++;
	}
	buf[num] = '\0';
	return num;
}

void handle_request(int client_fd, struct host_instance* hosts, struct host_list* host_lists) {
	int len = 0;
	char buf[256];
	char protocol[16];	// Request protocol
	char method[16];  // Request Method
	char host[MAX_DOMAIN_LENGTH];  // Host name
	char url[MAX_URL_LENGTH];  // Request Path
	char* url_path;  // Request Address
	char* params;		// Request params
	char path[MAX_URL_LENGTH];  // File Address

	int content_length = -1;

	len = get_require_line(client_fd, buf, sizeof(buf));
	if (len == 0) {
		printf("request format error\n");
		return;
	}
	sscanf(buf, "%s %s %s", method, url, protocol);
	printf("%s %s %s\n", method, url, protocol);

	const char q[] = "?";
	url_path = strtok(url, q);
	params = strtok(NULL, q);
	if (params != NULL) {
		// printf("params: %s\n", params);
	}
	// printf("url_path: %s\n", url_path);

	// Find virtual host name
	// printf("request body:\n%s\n", buf);
	host[0] = '\0';
	do{
		len = get_require_line(client_fd, buf, sizeof(buf));
		if (host[0] == '\0') {
			sscanf(buf, "Host: %s", host);
			if (host[0] == '\0') {
				sscanf(buf, "host: %s", host);
			}
		}
		printf("%s\n", buf);
		if (content_length == -1) {
			sscanf(buf, "Content-Length: %d", &content_length);
		}
	}while (len > 0);
	sscanf(host, "%[^:]", host);
	printf("host: %s\n", host);
	printf("content_length: %d\n", content_length);

	// Matching virtual host id
	int i = host_lists->id;
	while(host_lists) {
		if (strcmp(host, host_lists->server_name) == 0) {
			i = host_lists->id;
			break;
		}
		if (host_lists->server_name[0] == '_' && host_lists->server_name[1] == '\0') i = host_lists->id;
		host_lists = host_lists->next;
	}

	// Get request file and path
	strcpy(path, hosts[i].root);
	strcat(path, url_path);
	printf("path: %s\n", path);

	if (strcmp(method, POST) == 0) {
		printf("handling post...\n");
		int code = handle_post(client_fd, path, content_length, url_path);
		printf("handled post %d\n", code);
		return;
	}

	if (strcmp(method, DELETE) == 0) {
		printf("handling delete...\n");
		int code = handle_delete(client_fd, path);
		printf("handled delete %d\n", code);
		return;
	}

	if (strcmp(method, PUT) == 0) {
		printf("handling put...\n");
		int code = handle_put(client_fd, path, content_length);
		printf("handled put %d\n", code);
		return;
	}

	if (strcmp(method, OPTIONS) == 0) {
		printf("handling options...\n");
		int code = handle_options(client_fd, path);
		printf("handled options %d\n", code);
		return;
	}

	if (strcmp(method, OPTIONS) == 0) {
		printf("handling options...\n");
		int code = handle_options(client_fd, path);
		printf("handled options %d\n", code);
		return;
	}

	// Determine if a file or directory exists
	if (stat(path, &st) == -1){
		printf("stat %s find failed.\n", path);
		handle_response(client_fd, 404, "./error/404.html");
	}else {
		if (S_ISDIR(st.st_mode)){  // If it is a directory, get the index.html file of the corresponding directory
			strcat(path, hosts[i].index);
		}
		handle_response(client_fd, 200, path);
	}
}

int handle_response(int client_fd, int code, char* path) {
	int size;
	char buf[MAX_TCP_PACKAGE_SIZE];
	char temp[MAX_TCP_PACKAGE_SIZE];
	char data[MAX_TCP_PACKAGE_SIZE];
	struct stat st;

	// Open the file and get the file size
	FILE* resourse = fopen(path, "r");
	if (resourse == NULL){
		printf("file %s open failed. \n", path);
		if (code == 404) {
			strcpy(buf, status_codes_404(HTTP/1.1));
			strcat(buf, "Server: Nginxxx\r\n");
			strcat(buf, "Content-Type: text/plain; charset=utf-8\r\n");
			sprintf(temp, "Content-Length: %d\r\n\r\n404 Not Found", 13);
			strcat(buf, temp);
			return send(client_fd, buf, strlen(buf), 0);
		}
		return handle_response(client_fd, 404, "./error/404.html");
	}
	fstat(fileno(resourse), &st);
	size = st.st_size;
	if (size == 0) {
		fseek(resourse, 0, SEEK_END);
		size = ftell(resourse);
		fseek(resourse, 0, SEEK_SET);
	}

	// Write status line
	switch (code) {
		case 200:
			strcpy(buf, status_codes_200(HTTP/1.1));
			break;
		case 400:
			strcpy(buf, status_codes_400(HTTP/1.1));
			break;
		case 404:
			strcpy(buf, status_codes_404(HTTP/1.1));
			break;
		case 500:
			strcpy(buf, status_codes_500(HTTP/1.1));
			break;
		case 501:
			strcpy(buf, status_codes_501(HTTP/1.1));
			break;
	}

	// Write message headers
	strcat(buf, "Server: Nginxxx\r\n");
	const char point = '.';
	char* suffix_name;
	suffix_name = strrchr(path, point);
	if (strcmp(suffix_name, ".html") == 0) {
		strcat(buf, "Content-Type: text/html; charset=utf-8\r\n");
	} else if (strcmp(suffix_name, ".htm") == 0) {
		strcat(buf, "Content-Type: text/html; charset=utf-8\r\n");
	} else if (strcmp(suffix_name, ".txt") == 0) {
		strcat(buf, "Content-Type: text/plain; charset=utf-8\r\n");
	} else if (strcmp(suffix_name, ".jpg") == 0) {
		strcat(buf, "Content-Type: image/jpeg\r\n");
	} else if (strcmp(suffix_name, ".png") == 0) {
		strcat(buf, "Content-Type: image/png\r\n");
	} else if (strcmp(suffix_name, ".gif") == 0) {
		strcat(buf, "Content-Type: image/gif\r\n");
	}
	strcat(buf, "Connection: keep-alive\r\n");
	sprintf(temp, "Content-Length: %d\r\n\r\n", size);
	strcat(buf, temp);

	// printf("write:\n%s\n", buf);
	// Sending headers
	send(client_fd, buf, strlen(buf), 0);
	do{
		int len = fread(data, 1, MAX_TCP_PACKAGE_SIZE, resourse);
		write(client_fd, data, MAX_TCP_PACKAGE_SIZE);
		// printf("%s", data);
	}while (!feof(resourse));
	return 200;
}
