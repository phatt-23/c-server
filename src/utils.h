#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stdlib.h>

int get_url_from_https_request(char* dest_url_buffer, 
                               const char* request_buffer);

int send_file(int dest_fd, 
              const char* file_path);

int get_file_size(const char* file_path);

bool streq(const char* s1, 
           const char* s2);

int get_http_time_format(char* time_buffer, 
                         size_t size);

#endif // !UTILS_H_
