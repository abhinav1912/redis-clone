#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
#include<cstring>
#include<vector>
#include<poll.h>
#include "Node.h"

#define PORT 8080
#define MAXCONN 3
#define MAX_MESSAGE_LEN 4096

enum {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2
};

struct Connection
{
    int state = STATE_REQ;
    int fd = -1;
    // read buffer
    size_t readbuf_size = 0;
    uint8_t readbuf[4 + MAX_MESSAGE_LEN];
    // write buffer
    size_t writebuf_size = 0;
    size_t writebuf_sent = 0;
    uint8_t writebuf[4 + MAX_MESSAGE_LEN];
};


static void log_error_and_abort(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

class Server: Node {
    int single_request(int connfd) {
        char rbuf[4 + MAX_MESSAGE_LEN + 1] = {};
        errno = 0;
        int rv = read_full(connfd, rbuf, 4);
        if (rv) {
            if (errno) {
                log_message("read() error");
            } else {
                log_message("EOF");
            }
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
        printf("Client %d says: %s\n", connfd, &rbuf[4]);

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

        std::vector<Connection *> connection_mapping;
        set_fd_as_nonblocking(sockfd);
        std::vector<struct pollfd> poll_args;

        while (true) {
            poll_args.clear();
            // listening socket is put first
            struct pollfd polling_fd = {sockfd, POLLIN, 0};
            poll_args.push_back(polling_fd);

            // iterate over fds
            for (Connection* connection: connection_mapping) {
                if (!connection) {
                    continue;
                }
                struct pollfd pfd = {};
                pfd.fd = connection->fd;
                pfd.events = (connection->state == STATE_REQ) ? POLLIN : POLLOUT;
                pfd.events |= POLLERR;
                poll_args.push_back(pfd);
            }

            // poll for active fds
            int recv = poll(poll_args.data(), poll_args.size(), 1000);
            if (recv < 0) {
                log_error_and_abort("polling error");
            }

            // process active connections
            for (size_t i = 1; i < poll_args.size(); ++i) {
                if (poll_args[i].revents) {
                    Connection *connection = connection_mapping[poll_args[i].fd];
                    // handle connection
                    if (connection->state == STATE_END) {
                        // destroy this connection
                        connection_mapping[connection->fd] = NULL;
                        close(connection->fd);
                        free(connection);
                    }
                }
            }

            // accept new connection if listening fd is active
            if (poll_args[0].revents) {
                // handle new connection
            }
        }
    }
};

int main() {
    Server server = Server();
    server.listen_for_connections();
    return 0;
}