#include<stdio.h>

class Node {
    public:
        int get_socket(bool set_reuse_opt);
        sockaddr_in get_address(int port, int address);
        int read_full(int fd, char *buf, size_t n);
        int write_full(int fd, char *buf, size_t n);
        void log_message(const char *msg);
        void log_error_and_abort(const char *msg);
};