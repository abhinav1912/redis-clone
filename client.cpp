#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>

#define PORT 8080
#define MESSAGE_LEN 64

static void log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_error_and_abort("Error while opening socket");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int recv = connect(sockfd, (const sockaddr *) &addr, sizeof(addr));
    if (recv < 0) {
        log_error_and_abort("Error while connecting to server");
    }

    char msg[] = "Hello Server";
    write(sockfd, msg, sizeof(msg));

    char read_buffer[MESSAGE_LEN] = {};
    int n = read(sockfd, read_buffer, MESSAGE_LEN);

    if (n<0) {
        log_error_and_abort("Error while reading from server");
    }
    printf("Message from server: %s\n", read_buffer);
    close(sockfd);

    return 0;
}