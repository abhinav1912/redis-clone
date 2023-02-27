#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include "Node.h"

void Node::log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int Node::read_full(int fd, char *buf, size_t n) {
    // TODO: complete implementation
    return 0;
}

int Node::write_full(int fd, char *buf, size_t n) {
    // TODO: complete implementation
    return 0;
}

int Node::get_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), opt = 1;
    if (sockfd < 0) {
        log_error_and_abort("Error while opening socket");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        log_error_and_abort("Error setting socket options");
    }
    return sockfd;
}

sockaddr_in Node::get_address(int port, int address) {
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(address);
    return addr;
}