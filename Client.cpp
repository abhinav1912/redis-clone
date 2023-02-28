#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
#include "Node.h"
#define PORT 8080
#define MESSAGE_LEN 64

class Client: Node {
    public:
    void send_message_to_server() {
        int sockfd = get_socket(false);
        struct sockaddr_in addr = get_address(PORT, INADDR_LOOPBACK);

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
    }
};

int main() {
    Client client = Client();
    client.send_message_to_server();
    return 0;
}