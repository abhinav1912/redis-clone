#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<cassert>
#include<fcntl.h>
#include "Node.h"

void Node::log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int Node::read_full(int fd, char *buf, size_t n) {
    while (n>0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;
        }
        assert((size_t)rv <= n);
        buf += rv;
        n -= rv;
    }
    return 0;
}

int Node::write_full(int fd, char *buf, size_t n) {
    while (n>0) {
        ssize_t rv = write(fd, buf, n);
        if (rv < 0) {
            return -1;
        }
        assert((size_t)rv <= n);
        buf += rv;
        n -= rv;
    }
    return 0;
}

int Node::get_socket(bool set_reuse_opt) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), opt = 1;
    if (sockfd < 0) {
        log_error_and_abort("Error while opening socket");
    }

    if (set_reuse_opt) {
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
           log_error_and_abort("Error setting socket options");
        }
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

void Node::log_message(const char *msg) {
    fprintf(stdout, "%s\n", msg);
}

void Node::set_fd_as_nonblocking(int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        log_error_and_abort("fcntl get error");
    }
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    if (errno) {
        log_error_and_abort("fcntl set error");
    }
}