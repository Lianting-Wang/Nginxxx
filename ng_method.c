#include "ng_method.h"
#include "ng_file_util.h"

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
    char response_header[1024];
    char temp[64];

    FILE * fp = fopen(path, "r");
    if (fp) {
        printf("File alreayd exists\n");
        // TODO send error for there already exists a file
        close(fp);
    }
    int fd = open(path, O_CREAT);
    if (fd == -1) {
        printf("Failed to create and open a new file at location\n");
        strcpy(response_header, status_codes_500(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");

        strcat(response_header, "Connection: keep-alive\r\n");

        sprintf(temp, "Content-Length: %d\r\n\r\n", 16);
        strcat(response_header, temp);
        send(client_fd, response_header, strlen(response_header), 0);

        char error_message[] = "500 Server Error";
        send(client_fd, error_message, strlen(error_message), 0);

        return -1;
    }

    
    int len = 0;
    do {
        char buffer[1024];
        len = read(client_fd, buffer, 1024);
        len = write(fd, buffer, len);
        prtinf("Wrote %d bytes\n", len);
    } while (len > 0);
    close(fd);
    printf("Finished writing to file");

    // TODO send client back the status and code
    strcpy(response_header, status_codes_201(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");

    strcat(response_header, "Location "); // specify location
    strcat(response_header, path);
    strcat(response_header, "\r\n");

    append_content_type(response_header, path);
    strcat(response_header, "Connection: keep-alive\r\n");

    sprintf(temp, "Content-Length: %d\r\n\r\n", 0);
    strcat(response_header, temp);

    send(client_fd, response_header, strlen(response_header), 0);
    return 0 /* TODO change this status code */;
}
