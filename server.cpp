#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>

#define PORT 8080
#define MAXCONN 3
#define MESSAGE_LEN 64

static void log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void handle_connection(int connfd) {
    char readbuf[MESSAGE_LEN] = {};
    int n = read(connfd, readbuf, MESSAGE_LEN);
    if (n < 0) {
        perror("Error while reading");
    }
    printf("Client %d's message: %s", connfd, readbuf);
    char response_buf[] = "Response!";
    write(connfd, response_buf, sizeof(response_buf));
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), opt = 1;
    if (sockfd < 0) {
     log_error_and_abort("Error while opening socket");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
     log_error_and_abort("Error setting socket options");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (const sockaddr *) &addr, sizeof(addr)) < 0) {
     log_error_and_abort("Error while binding");
        
    }

    int recv = listen(sockfd, MAXCONN);
    if (recv < 0) {
     log_error_and_abort("Error opening listener");
    }

    while (true) {
        struct sockaddr client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(sockfd, (sockaddr *) &client_addr, (socklen_t *) &socklen);
        if (connfd < 0) {
         log_error_and_abort("Error while accepting connection");
            continue;
        }
        handle_connection(connfd);
        close(connfd);
    }
    return 0;
}