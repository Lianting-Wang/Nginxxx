#include "ng_method.h"
#include "ng_file_util.h"

// LIKELY WILL END UP UNUSED
int handle_get(int client_fd, char* path) {
    int size;
    char response_header[1024];
    char temp[64];
    char data[1024];
    struct stat st;

    FILE* resource = fopen(path, "r");

    // Resource not found, handle 404 or redirect
    if (resource == NULL) {
        if (strcmp(path, "./error/404.html") == 0) {
            int backup_result = backup_404(client_fd);
            if (backup_result != 0) {
                printf("backup 404 failed.\n");
            }
            return backup_result;
        }
        else {
            return handle_get(client_fd, "./error/404.html");
        }
    }

    fstat(fileno(resource), &st);
    size = st.st_size;
    if (size == 0) { // cannot find file size directly, manually get instead
        fseek(resource, 0, SEEK_END);
        size = ftell(resource);
        fseek(resource, 0, SEEK_SET);
    }

    // Write status line
    if (strcmp(path, "./error/404.html") == 0) {
        strcpy(response_header, status_codes_404("HTTP/1.1"));
    }
    else {
        strcpy(response_header, status_codes_200("HTTP/1.1"));
    }

    // Write message headers
    strcat(response_header, "Server: Nginxxx\r\n");
    append_content_type(response_header, path);
    strcat(response_header, "Connection: keep-alive\r\n");
    sprintf(temp, "Content-Length: %d\r\n\r\n", size);
    strcat(response_header, temp);
    send(client_fd, response_header, strlen(response_header), 0);

	do{
		int len = fread(data, 1, MAX_TCP_PACKAGE_SIZE, resource);
		write(client_fd, data, MAX_TCP_PACKAGE_SIZE);
		// printf("%s", data);
	}while (!feof(resource));
}

int backup_404(int client_fd) {
    char response_header[1024];
    int return_code = 0;
    char temp[64];
    char error_message[] = "404 Not Found";

    // NOT FOUND
    strcpy(response_header, status_codes_404(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");
    strcat(response_header, "Connection: keep-alive\r\n");
    sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
    strcat(response_header, temp);
    if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
    if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
    return return_code;
}

int handle_post(int client_fd, char* path, int content_length, char * url_path) {
    char response_header[1024];
    char temp[64];
    int return_code = 0;

    // BAD REQUEST
    if (content_length < 0) {
        return_code = BAD_ARG;
        char error_message[] = "411 Length Required";

        strcpy(response_header, status_codes_411(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    // Location occupied
    struct stat st;
    if (stat(path, &st) == 0) {
        return_code = FS_ERR;
        char error_message[] = "409 Conflict";

        printf("entity already exists at requested location.\n");
        strcpy(response_header, status_codes_409(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");

        strcat(response_header, "Connection: keep-alive\r\n");

        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        printf("Constructed header: %s\n", response_header);
        printf("Constructed Message: %s\n", error_message);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        printf("Finished sending 409\n");
        return return_code;
    }

    int mkdir_status = mkdir_parent(path);

    printf("mkdir status: %d\n", mkdir_status & INTR_ERR);
    printf("mkdir status: %d\n", mkdir_status & FS_ERR);
    
    // DIR creation failed
    if ((mkdir_status & INTR_ERR) == INTR_ERR) {
        printf("mkdir failed.\n");
        return_code = INTR_ERR;
        return_code |= send_internal_server_err(client_fd);
        return return_code;
    }

    // DIR occupied
    if ((mkdir_status & FS_ERR) == FS_ERR) {
        printf("mkdir encounter conflict.\n");
        return_code = FS_ERR;
        char error_message[] = "409 Conflict";

        printf("entity already exists at requested location.\n");
        strcpy(response_header, status_codes_409(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");

        strcat(response_header, "Connection: keep-alive\r\n");

        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    int fd = open(path, O_RDWR | O_CREAT, 0777);
    if (fd == -1) {
        return_code = INTR_ERR;
        char error_message[] = "500 Server Error";

        printf("Failed to create and open a new file at location\n");
        strcpy(response_header, status_codes_500(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");

        strcat(response_header, "Connection: keep-alive\r\n");

        sprintf(temp, "Content-Length: %d\r\n\r\n", 16);
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        
        return return_code;
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

    // ALL GOOD, SEND 201 CREATED
    strcpy(response_header, status_codes_201(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");

    strcat(response_header, "Location: "); // specify location
    strcat(response_header, url_path);
    strcat(response_header, "\r\n");

    strcat(response_header, "Connection: keep-alive\r\n");

    sprintf(temp, "Content-Length: %d\r\n\r\n", 0);
    strcat(response_header, temp);

    printf("Constructed header: %s\n", response_header);

    if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
    
    char message[] = "201 Created";
    if (send(client_fd, message, strlen(message) + 1, 0) == -1) return_code |= CONN_ERR;
    return return_code;
}


int handle_delete(int client_fd, char* path) {
    int return_code = 0;
    char response_header[1024];
    char temp[64];

    struct stat st;
    int stat_result = stat(path, &st);

    // File doesnt exists
    if (stat_result == -1) {
        printf("stat %s find failed.\n", path);
        return_code = FS_ERR;
        char error_message[] = "404 Not Found";
        strcpy(response_header, status_codes_404(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    // DELETE DIR IS NOT AN OPTION
    // 405 Method Not Allowed
    if (S_ISDIR(st.st_mode)) { // If it is a directory, get the index.html file of the corresponding directory
        return_code = FS_ERR;
        char error_message[] = "405 Method Not Allowed";
        strcpy(response_header, status_codes_405(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        /************************ IMPORTANT **************************/
        strcat(response_header, "Allow: GET, HEAD\r\n");
        /*************************************************************/
        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    // INTERNAL ERROR
    if (unlink(path) == -1) { 
        return_code = INTR_ERR;

        char error_message[] = "500 Internal Server Error";

        strcpy(response_header, status_codes_500(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");

        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));

        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    // ALL GOOD, send back 204
    strcpy(response_header, status_codes_204(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");
    strcat(response_header, "Connection: keep-alive\r\n");

    sprintf(temp, "Content-Length: %d\r\n\r\n", 0);
    strcat(response_header, temp);

    if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
    if (send(client_fd, "", 1, 0) == -1) return_code |= CONN_ERR;
    return return_code;
}

int handle_options(int client_fd, char* path) {
    int return_code = 0;
    char response_header[1024];
    char temp[64];

    strcpy(response_header, status_codes_204(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");
    strcat(response_header, "Connection: keep-alive\r\n");

    struct stat st;
    int stat_result = stat(path, &st);

    // URL doesnt exists
    if (stat_result == -1) {
        strcat(response_header, "Allow: HEAD, POST, OPTIONS\r\n");
    }

    if (stat_result == 0 && S_ISDIR(st.st_mode) == 0) { // If it is a directory, get the index.html file of the corresponding directory
        strcat(response_header, "Allow: GET, HEAD, OPTIONS\r\n");
    }

    if (stat_result == 0 && S_ISREG(st.st_mode) == 0) { // If it is a file, get the file
        strcat(response_header, "Allow: GET, HEAD, PUT, DELETE, OPTIONS\r\n");
    }

    sprintf(temp, "Content-Length: %d\r\n\r\n", 0);
    strcat(response_header, temp);
    if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
    if (send(client_fd, "", 1, 0) == -1) return_code |= CONN_ERR;
    
    return return_code;
}

int handle_put(int client_fd, char * path, int content_length) {
    int return_code = 0;
    char response_header[1024];
    char temp[64];

    if (content_length < 0) {
        return_code = BAD_ARG;
        char error_message[] = "411 Length Required";

        strcpy(response_header, status_codes_411(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    struct stat st;
    int stat_result = stat(path, &st);

    // 405 Method Not Allowed
    if (stat_result == 0 && S_ISDIR(st.st_mode)) { // If it is a directory, get the index.html file of the corresponding directory
        return_code = FS_ERR;
        char error_message[] = "405 Method Not Allowed";
        strcpy(response_header, status_codes_405(HTTP/1.1));
        strcat(response_header, "Server: Nginxxx\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        /************************ IMPORTANT **************************/
        strcat(response_header, "Allow: GET, HEAD\r\n");
        /*************************************************************/
        sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));
        strcat(response_header, temp);
        if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
        if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
        return return_code;
    }

    // Need to create a new file
    if (stat_result == -1) {

    }

}

int send_internal_server_err(int client_fd) {
    int return_code = 0;
    char response_header[1024];
    char temp[64];

    char error_message[] = "500 Internal Server Error";

    strcpy(response_header, status_codes_500(HTTP/1.1));
    strcat(response_header, "Server: Nginxxx\r\n");
    strcat(response_header, "Connection: keep-alive\r\n");

    sprintf(temp, "Content-Length: %ld\r\n\r\n", strlen(error_message));

    strcat(response_header, temp);
    if (send(client_fd, response_header, strlen(response_header), 0) == -1) return_code |= CONN_ERR;
    if (send(client_fd, error_message, strlen(error_message) + 1, 0) == -1) return_code |= CONN_ERR;
    return return_code;
}
