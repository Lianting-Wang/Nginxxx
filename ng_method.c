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

int handle_post(int client_fd, char * path, int content_length) {
    char response_header[1024];
    char temp[64];

    if (content_length < 0) {
        // TODO send error for either no or invalid content-length
        return -1;
    }

    FILE * fp = fopen(path, "r");
    if (fp) {
        printf("File already exists\n");
        // TODO send error for there already exists a file
        fclose(fp);
    }
    int fd = open(path, O_RDWR | O_CREAT);
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

    
    int read_len = 0;
    int write_len = 0;
    int total = 0;
    do {
        char buffer[1024];

        read_len = read(client_fd, buffer, 1024);
        printf("Read %d bytes\n", read_len);
        write_len = write(fd, buffer, read_len);
        printf("Wrote %d bytes\n", write_len);
        total += write_len;
    } while (write_len > 0 && total < content_length);
    close(fd);
    printf("Finished writing to file\n");

    // TODO send client back the status and code
    strcpy(response_header, status_codes_201(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");

    strcat(response_header, "Location: "); // specify location
    strcat(response_header, path);
    strcat(response_header, "\r\n");

    append_content_type(response_header, path);
    strcat(response_header, "Connection: keep-alive\r\n");

    sprintf(temp, "Content-Length: %d\r\n\r\n", 0);
    strcat(response_header, temp);

    printf("Constructed header: %s\n", response_header);

    send(client_fd, response_header, strlen(response_header), 0);
    return 0 /* TODO change this status code */;
}


int handle_delete(int client_fd, char * path) {
    struct stat st;
    int stat_result = stat(path, &st);
    if (stat_result == -1) {
		printf("stat %s find failed.\n", path);
        // TODO send error for file does not exists
        return -1;
	} 
    if (S_ISDIR(st.st_mode)) { // If it is a directory, get the index.html file of the corresponding directory
        // TODO send error for the destination is a folder
    }
    if (unlink(path) == -1) {
        // TODO send 5x for failure to delete the file
    }
    // TODO create and send back HTTP response
}
