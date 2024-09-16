#include <time.h>
#include <asm-generic/socket.h>
#include <string.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>

bool streq(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create a socket");
        exit(1);
    }
    
    int sock_option = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sock_option, sizeof(sock_option))) {
        perror("Failed to set socket options!");
        exit(1);
    }

    printf("Socket option: %d\n", sock_option);

    printf("Server TCP socket created: %d\n", server_fd);

    char* address_domain = "127.0.0.1"; // localhost
    // char* address_domain = "192.168.100.170";
    int address_port = 6969; 

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(address_domain),
        .sin_port = htons(address_port),
    };

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Server socket failed to bind!");
        exit(1);
    }

    printf("Server socket bound to: %s:%d\n", address_domain, address_port);

    if (listen(server_fd, 10) < 0) {
        perror("Server socket failed to start listening");
        exit(1);
    }

    printf("Server started listening!\n");
    
    while (1) {
        struct sockaddr_in client_address;
        int client_address_len = 0;
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&client_address_len);
        
        char client_buffer[1024] = {0};
        read(client_fd, client_buffer, sizeof(client_buffer));
        
        char* str_start = strchr(client_buffer, ' ') + 1;
        char* str_end = strchr(str_start, ' ');
        char route[1024];
        strlcpy(route, str_start, str_end - str_start + 1);

        printf("Client request: %s\n", route); 
       
        time_t time_now = time(NULL);
        struct tm* timer = gmtime(&time_now);
        char time_buffer[1024] = {0};
        strftime(time_buffer, sizeof(time_buffer), "%a, %d %b %Y %I:%M:%S GMT", timer);

        char* response = (char*)malloc(4096);
        sprintf(response, "HTTP/1.1 200 OK\nDate: %s\nServer: MyServer\nLast-Modified: %s\nContent-Type: text/html\nConnection: Closed\n\n", time_buffer, time_buffer);

        if (streq(route, "/")) 
        {
            char file_content_buffer[1024] = {0};
            FILE* file_fd = fopen("./routes/index.html", "r");
            while (fgets(file_content_buffer, sizeof file_content_buffer, file_fd) != NULL) {
                strcat(response, file_content_buffer);
                printf("File content: %s", file_content_buffer);
            }

            write(client_fd, response, strlen(response));
        } 
        else if (streq(route, "/quit")) 
        {
            strcat(response, "<h1>Shutting down the server!</h1>");
            write(client_fd, response, strlen(response));
            break;
        }
        else if (streq(route, "/home")) {
            strcat(response, "<h1>Welcome home!</h1>");
            write(client_fd, response, strlen(response));
        }

        free(response);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
