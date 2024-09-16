#include "./utils.h"
#include <time.h>
#include <asm-generic/socket.h>
#include <string.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>

bool streq(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

int get_file_size(const char* file_path) {
    FILE* file_fd = fopen(file_path, "r");
    if (file_fd == NULL) {
        printf("%s:%d: Failed to fopen \"%s\"\n", __FILE__, __LINE__, file_path);
        fclose(file_fd);
        return 0;
    }
    if (fseek(file_fd, 0, SEEK_END) == -1) {
        printf("%s:%d: Failed to fseek \"%s\"\n", __FILE__, __LINE__, file_path);
        fclose(file_fd);
        return 0;
    }
    int file_size = ftell(file_fd);
    if (file_size == -1) {
        printf("%s:%d: Failed to ftell \"%s\"\n", __FILE__, __LINE__, file_path);
        fclose(file_fd);
        return 0;
    }
    fclose(file_fd);
    return file_size;
}

int send_file(int dest_fd, const char* file_path) {
    int err = 0;
    int opened_file_fd = open(file_path, O_RDONLY);
    if (opened_file_fd == -1) {
        printf("%s:%d: Failed to open \"%s\"\n", __FILE__, __LINE__, file_path);
        err = close(opened_file_fd);
        return err;
    }
    if ((err = sendfile(dest_fd, opened_file_fd, 0, get_file_size(file_path))) == -1) {
        printf("%s:%d: Failed to sendfile \"%s\"\n", __FILE__, __LINE__, file_path);
        err = close(opened_file_fd);
        return err;
    }
    err = close(opened_file_fd);
    return err;
}

int get_url_from_https_request(char* dest_url_buffer, const char* request_buffer) {
    char* str_start = strchr(request_buffer, ' ') + 1;
    if (str_start == NULL) {
        printf("%s:%d: HTTP request has wrong format!\n", __FILE__, __LINE__);
        return -1;
    }
    char* str_end = strchr(str_start, ' ');
    if (str_end == NULL) {
        printf("%s:%d: HTTP request has wrong format!\n", __FILE__, __LINE__);
        return -1;
    }
    strlcpy(dest_url_buffer, str_start, str_end - str_start + 1);
    
    if (strlen(dest_url_buffer) == 1)
        return 0;

    if (dest_url_buffer[strlen(dest_url_buffer) - 1] == '/')
       dest_url_buffer[strlen(dest_url_buffer) - 1] = 0;

    return 0;
}

int get_http_time_format(char* time_buffer, size_t size) {
    time_t time_now = time(NULL);
    struct tm* timer = gmtime(&time_now);
    return strftime(time_buffer, size, "%a, %d %b %Y %I:%M:%S GMT", timer);
}
