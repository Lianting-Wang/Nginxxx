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
		int code = handle_put(client_fd, path, content_length, url_path);
		printf("handled put %d\n", code);
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
		handle_response(client_fd, 404, "./error/404.html", method);
	}else {
		if (S_ISDIR(st.st_mode)){  // If it is a directory, get the index.html file of the corresponding directory
			strcat(path, hosts[i].index);
		}
		handle_response(client_fd, 200, path, method);
	}
}

int handle_response(int client_fd, int code, char* path, char method[]) {
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
		return handle_response(client_fd, 404, "./error/404.html", method);
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
		case 100:
			strcpy(buf, status_codes_100(HTTP/1.1));
			break;
		case 200:
			strcpy(buf, status_codes_200(HTTP/1.1));
			break;
		case 201:
			strcpy(buf, status_codes_201(HTTP/1.1));
			break;
		case 202:
			strcpy(buf, status_codes_202(HTTP/1.1));
			break;
		case 204:
			strcpy(buf, status_codes_204(HTTP/1.1));
			break;
		case 206:
			strcpy(buf, status_codes_206(HTTP/1.1));
			break;
		case 301:
			strcpy(buf, status_codes_301(HTTP/1.1));
			break;
		case 302:
			strcpy(buf, status_codes_302(HTTP/1.1));
			break;
		case 303:
			strcpy(buf, status_codes_303(HTTP/1.1));
			break;
		case 304:
			strcpy(buf, status_codes_304(HTTP/1.1));
			break;
		case 307:
			strcpy(buf, status_codes_307(HTTP/1.1));
			break;
		case 308:
			strcpy(buf, status_codes_308(HTTP/1.1));
			break;
		case 400:
			strcpy(buf, status_codes_400(HTTP/1.1));
			break;
		case 401:
			strcpy(buf, status_codes_401(HTTP/1.1));
			break;
		case 403:
			strcpy(buf, status_codes_403(HTTP/1.1));
			break;
		case 404:
			strcpy(buf, status_codes_404(HTTP/1.1));
			break;
		case 406:
			strcpy(buf, status_codes_406(HTTP/1.1));
			break;
		case 407:
			strcpy(buf, status_codes_407(HTTP/1.1));
			break;
		case 409:
			strcpy(buf, status_codes_409(HTTP/1.1));
			break;
		case 410:
			strcpy(buf, status_codes_410(HTTP/1.1));
			break;
		case 412:
			strcpy(buf, status_codes_412(HTTP/1.1));
			break;
		case 416:
			strcpy(buf, status_codes_416(HTTP/1.1));
			break;
		case 418:
			strcpy(buf, status_codes_418(HTTP/1.1));
			break;
		case 451:
			strcpy(buf, status_codes_451(HTTP/1.1));
			break;
		case 500:
			strcpy(buf, status_codes_500(HTTP/1.1));
			break;
		case 501:
			strcpy(buf, status_codes_501(HTTP/1.1));
			break;
		case 502:
			strcpy(buf, status_codes_502(HTTP/1.1));
			break;
		case 503:
			strcpy(buf, status_codes_503(HTTP/1.1));
			break;
		case 504:
			strcpy(buf, status_codes_504(HTTP/1.1));
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
	} else if (strcmp(suffix_name, ".pdf") == 0) {
		strcat(buf, "Content-Type: application/pdf\r\n");
	} else if (strcmp(suffix_name, ".js") == 0) {
		strcat(buf, "Content-Type: application/javascript\r\n");
	} else if (strcmp(suffix_name, ".json") == 0) {
		strcat(buf, "Content-Type: application/json\r\n");
	} else if (strcmp(suffix_name, ".xml") == 0) {
		strcat(buf, "Content-Type: application/xml\r\n");
	} else if (strcmp(suffix_name, ".zip") == 0) {
		strcat(buf, "Content-Type: application/zip\r\n");
	} else if (strcmp(suffix_name, ".tiff") == 0) {
		strcat(buf, "Content-Type: image/tiff\r\n");
	} else if (strcmp(suffix_name, ".css") == 0) {
		strcat(buf, "Content-Type: text/css\r\n");
	} else if (strcmp(suffix_name, ".csv") == 0) {
		strcat(buf, "Content-Type: text/csv\r\n");
	} else if (strcmp(suffix_name, ".mpeg") == 0) {
		strcat(buf, "Content-Type: video/mpeg\r\n");
	} else if (strcmp(suffix_name, ".mp4") == 0) {
		strcat(buf, "Content-Type: video/mp4\r\n");
	} else if (strcmp(suffix_name, ".qt") == 0) {
		strcat(buf, "Content-Type: video/quicktime\r\n");
	} else if (strcmp(suffix_name, ".wmv") == 0) {
		strcat(buf, "Content-Type: video/x-ms-wmv\r\n");
	} else if (strcmp(suffix_name, ".avi") == 0) {
		strcat(buf, "Content-Type: video/x-msvideo\r\n");
	} else if (strcmp(suffix_name, ".flv") == 0) {
		strcat(buf, "Content-Type: video/x-flv\r\n");
	} else if (strcmp(suffix_name, ".webm") == 0) {
		strcat(buf, "Content-Type: video/webm\r\n");
	} else if (strcmp(suffix_name, ".apk") == 0) {
		strcat(buf, "Content-Type: application/vnd.android.package-archive\r\n");
	} else if (strcmp(suffix_name, ".odt") == 0) {
		strcat(buf, "Content-Type: application/vnd.oasis.opendocument.text\r\n");
	} else if (strcmp(suffix_name, ".ods") == 0) {
		strcat(buf, "Content-Type: application/vnd.oasis.opendocument.spreadsheet\r\n");
	} else if (strcmp(suffix_name, ".odp") == 0) {
		strcat(buf, "Content-Type: application/vnd.oasis.opendocument.presentation\r\n");
	} else if (strcmp(suffix_name, ".odg") == 0) {
		strcat(buf, "Content-Type: application/vnd.oasis.opendocument.graphics\r\n");
	} else if (strcmp(suffix_name, ".xls") == 0) {
		strcat(buf, "Content-Type: application/vnd.ms-excel\r\n");
	} else if (strcmp(suffix_name, ".xlsx") == 0) {
		strcat(buf, "Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet\r\n");
	} else if (strcmp(suffix_name, ".ppt") == 0) {
		strcat(buf, "Content-Type: application/vnd.ms-powerpoint\r\n");
	} else if (strcmp(suffix_name, ".pptx") == 0) {
		strcat(buf, "Content-Type: application/vnd.openxmlformats-officedocument.presentationml.presentation\r\n");
	} else if (strcmp(suffix_name, ".doc") == 0) {
		strcat(buf, "Content-Type: application/msword\r\n");
	} else if (strcmp(suffix_name, ".docx") == 0) {
		strcat(buf, "Content-Type: application/vnd.openxmlformats-officedocument.wordprocessingml.document\r\n");
	} else if (strcmp(suffix_name, ".xul") == 0) {
		strcat(buf, "Content-Type: application/vnd.mozilla.xul+xml\r\n");
	} else if (strcmp(suffix_name, ".jar") == 0) {
		strcat(buf, "Content-Type: application/java-archive\r\n");
	} else if (strcmp(suffix_name, ".bin") == 0) {
		strcat(buf, "Content-Type: application/octet-stream\r\n");
	} else if (strcmp(suffix_name, ".ogx") == 0) {
		strcat(buf, "Content-Type: application/ogg\r\n");
	} else if (strcmp(suffix_name, ".xhtml") == 0) {
		strcat(buf, "Content-Type: application/xhtml+xml\r\n");
	} else if (strcmp(suffix_name, ".swf") == 0) {
		strcat(buf, "Content-Type: application/x-shockwave-flash\r\n");
	} else if (strcmp(suffix_name, ".jsonld") == 0) {
		strcat(buf, "Content-Type: application/ld+json\r\n");
	} else if (strcmp(suffix_name, ".mpga") == 0) {
		strcat(buf, "Content-Type: audio/mpeg\r\n");
	} else if (strcmp(suffix_name, ".wma") == 0) {
		strcat(buf, "Content-Type: audio/x-ms-wma\r\n");
	} else if (strcmp(suffix_name, ".wav") == 0) {
		strcat(buf, "Content-Type: audio/x-wav\r\n");
	} else if (strcmp(suffix_name, ".ico") == 0) {
		strcat(buf, "Content-Type: image/x-icon\r\n");
	} else if (strcmp(suffix_name, ".djvu") == 0) {
		strcat(buf, "Content-Type: image/vnd.djvu\r\n");
	} else if (strcmp(suffix_name, ".svg") == 0) {
		strcat(buf, "Content-Type: image/svg+xml\r\n");
	} else if (strcmp(suffix_name, ".mjs") == 0) {
		strcat(buf, "Content-Type: text/javascript\r\n");
	}
	strcat(buf, "Connection: keep-alive\r\n");

	if (strcmp(method, GET) == 0) {
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
	} else if (strcmp(method, HEAD) == 0) {
		strcat(buf, "Content-Length: 0\r\n\r\n");
		send(client_fd, buf, strlen(buf), 0);
		send(client_fd, "", 1, 0);
	}
	return 200;
}
