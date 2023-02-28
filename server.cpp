#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
#include<cstring>
#include "Node.h"

#define PORT 8080
#define MAXCONN 3
#define MAX_MESSAGE_LEN 4096

static void log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

class Server: Node {
    int single_request(int connfd) {
        char rbuf[4 + MAX_MESSAGE_LEN + 1] = {};
        int rv = read_full(connfd, rbuf, 4);
        if (rv) {
            log_message("read() error");
            return rv;
        }
        uint32_t len = 0;
        memcpy(&len, rbuf, 4);
        if (len > MAX_MESSAGE_LEN) {
            log_message("Message too long");
            return -1;
        }

        rv = read_full(connfd, &rbuf[4], len);
        if (rv) {
            log_message("read() error");
            return rv;
        }

        // handle request
        rbuf[4 + len] = '\0';
        printf("Client %d says: %s\n", connfd, rbuf[4]);

        const char reply[] = "Served response!";
        char writebuf[4 + sizeof(reply)] = {};
        len = strlen(reply);
        memcpy(writebuf, &len, 4);
        memcpy(&writebuf[4], reply, len);
        return write_full(connfd, writebuf, 4 + len);
    };

    public:
    void listen_for_connections() {
        int sockfd = get_socket(true);
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
            while (true) { // serve just one client for now
                int err = single_request(connfd);
                if (err) {
                    break;
                }
            }
            close(connfd);
        }
    }
};

int main() {
    Server server = Server();
    server.listen_for_connections();
    return 0;
}