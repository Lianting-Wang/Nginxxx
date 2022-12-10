#include "ng_file_util.h"

int append_content_type(char * buf, char * path) {
    const char point = '.';
	char* suffix_name;
	suffix_name = strrchr(path, point);
	if (suffix_name == NULL) { // Consider this please
        strcat(buf, "Content-Type: text/plain; charset=utf-8\r\n");
    } else if (strcmp(suffix_name, ".html") == 0) {
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
    return 0;
}

// make directory recursively
int mkdir_parent(char * path) {
	char cp_path[1024];
	strcpy(cp_path, path);
	printf("cp_path: %s\n", cp_path);
	char * tracker = cp_path;
	while (*tracker != '\0') {
		if (*tracker == '/') {
			*tracker = '\0';
			struct stat st;
			if (stat(cp_path, &st) == -1) {
				if (mkdir(cp_path, 0777) == -1) {
					return INTR_ERR;
				}
			}
			else {
				if (S_ISDIR(st.st_mode) == 0) {
					return FS_ERR;
				}
			}
			*tracker = '/';
		}
		tracker++;
	}	
	return 0;
}
