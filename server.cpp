#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
#include "Node.h"

#define PORT 8080
#define MAXCONN 3
#define MESSAGE_LEN 64

static void log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

class Server: Node {
    void handle_connection(int connfd) {
        char readbuf[MESSAGE_LEN] = {};
        int n = read(connfd, readbuf, MESSAGE_LEN);
        if (n < 0) {
            perror("Error while reading");
            return;
        }
        printf("Client %d's message: %s\n", connfd, readbuf);
        char response_buf[] = "Response!";
        write(connfd, response_buf, sizeof(response_buf));
    };

    public:
    void listen_for_connections() {
        int sockfd = get_socket();
        struct sockaddr_in addr = get_address(PORT, INADDR_ANY);

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
    }
};

int main() {
    Server server = Server();
    server.listen_for_connections();
    return 0;
}