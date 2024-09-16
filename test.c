#include <stdio.h>
#include <time.h>

int main() {
    time_t time_now = time(NULL);
    printf("Time: %zu\n", time_now);

    struct tm* timer = gmtime(&time_now);

    char time_buffer[1024] = {0};
    strftime(time_buffer, sizeof(time_buffer), "%a, %d %b %Y %I:%M:%S GMT", timer);
    printf("%s\n", time_buffer);

    return 0;
}
