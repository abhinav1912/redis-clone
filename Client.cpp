#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
#include<cstring>
#include "Node.h"

#define PORT 8080
#define MAX_MESSAGE_LEN 4096

class Client: Node {
    public:
    int query(int fd, const char *text) {
        size_t len = strlen(text);
        if (len > MAX_MESSAGE_LEN) {
            log_message("Query too long");
            return -1;
        }        

        char readbuf[4 + MAX_MESSAGE_LEN + 1] = {}; // +1 since we need the EOF char at the end
        char writebuf[4 + MAX_MESSAGE_LEN] = {};

        memcpy(writebuf, &len, 4);
        memcpy(&writebuf[4], text, len);
        int err = write_full(fd, writebuf, len + 4);
        if (err) {
            return err;
        }

        err = read_full(fd, readbuf, 4);
        if (err) {
            log_message("read() error");
            return err;
        }
        
        memcpy(&len, readbuf, 4);
        if (len > MAX_MESSAGE_LEN) {
            log_message("Server response too long");
            return -1;
        }

        err = read_full(fd, &readbuf[4], len);
        if (err) {
            log_message("read() error");
            return err;
        }

        readbuf[4 + len] = '\0';
        printf("Server response: %s\n", &readbuf[4]);
        return 0;
    };

    void send_message_to_server() {
        int sockfd = get_socket(false);
        struct sockaddr_in addr = get_address(PORT, INADDR_LOOPBACK);

        int recv = connect(sockfd, (const sockaddr *) &addr, sizeof(addr));
        if (recv < 0) {
            log_error_and_abort("Error while connecting to server");
        }
        int err = query(sockfd, "query1");
        err = query(sockfd, "query2");
        err = query(sockfd, "new query!");
        close(sockfd);
    }
};

int main() {
    Client client = Client();
    client.send_message_to_server();
    return 0;
}