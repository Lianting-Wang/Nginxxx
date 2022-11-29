#include "ng_method.h"

char * handle_get(char * path) {
    int size;
    char buf[1024];
    char temp[64];
    char data[1024];
    struct stat st;
    FILE * resource = fopen(path, "r");
    
    if (resource == NULL) {
        printf("Failed to open file. \n", path);
        // TODO handle 404 not found here
    }
    fstat(fileno(resource), &st);
    size = st.st_size;
    if (size == 0) { // cannot find file size directly, manually get instead
        fseek(resource, 0, SEEK_END);
        size = ftell(resource);
        fseek(resource, 0, SEEK_SET);
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
	sprintf(temp, "Content-Length: %d\r\n", size);
	strcat(buf, temp);
    strcat(buf, "\r\n");
    // Finish writing message header


}

int handle_post(int client_fd, char * path) {
    FILE * fp = fopen(path, "r");
    if (fp) {
        // TODO send error for there already exists a file
        close(fp);
    }
    int fd = open(path, O_CREAT);
    if (fd == -1) {
        // TODO error, send 5xx error back
        return -1;
    }
    int len = 0;
    do {
        char buffer[1024];
        len = read(client_fd, buffer, 1024);
        len = write(fd, buffer, len);
    } while (len > 0);
    close(fd);

    // TODO send client back the status and code
    return 0 /* TODO change this status code */;
}
