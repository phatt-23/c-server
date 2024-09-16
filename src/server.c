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
#include <stdbool.h>
#include <fcntl.h>

#include "./utils.h"

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

    printf("Server TCP socket created: %d\n", server_fd);

    //char* address_domain = "127.0.0.1"; // localhost
    //char* address_domain = "192.168.100.170";
    char* address_domain = "0.0.0.0";
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
        int client_fd = accept(server_fd, 
                               (struct sockaddr*)&client_address, 
                               (socklen_t*)&client_address_len);
        
        char client_buffer[1024] = {0};
        if (read(client_fd, client_buffer, sizeof(client_buffer)) == -1) {
            perror("Reading to client file descriptor failed!");
            break;
        }
       
        char url_buffer[1024] = {0};
        get_url_from_https_request(url_buffer, client_buffer);
        printf("Client request URL: %s\n", url_buffer); 
       
        char time_buffer[512] = {0};
        get_http_time_format(time_buffer, sizeof time_buffer);

        char* response = (char*)malloc(4096);
        sprintf(response, "HTTP/1.1 200 OK\nDate: %s\nServer: MyServer\nLast-Modified: %s\nConnection: Closed\n", time_buffer, time_buffer);

        if (streq(url_buffer, "/quit")) 
        {
            strcat(response, "Content-Type: text/html\n\n");
            write(client_fd, response, strlen(response));
            send_file(client_fd, "./routes/quit/index.html");
            free(response);
            close(client_fd);
            break;
        }
        else if (streq(url_buffer, "/")) {
            strcat(response, "Content-Type: text/html\n\n");
            write(client_fd, response, strlen(response));
            send_file(client_fd, "./routes/index.html");
        } 
        else if (streq(url_buffer, "/home")) 
        {
            strcat(response, "Content-Type: text/html\n\n");
            write(client_fd, response, strlen(response));
            send_file(client_fd, "./routes/home/index.html");
        }
        else if (streq(url_buffer, "/css/style.css"))
        {
            strcat(response, "Content-Type: text/css\n\n");
            write(client_fd, response, strlen(response));
            send_file(client_fd, "./css/style.css");
        }
        else if (streq(url_buffer, "/favicon.ico"))
        {
            strcat(response, "\n");
            write(client_fd, response, strlen(response));
            send_file(client_fd, "./public/favicon.ico");
        }

        free(response);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
