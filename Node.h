#include<stdio.h>

class Node {
    public:
        void log_error_and_abort(const char *msg);
        int read_full(int fd, char *buf, size_t n);
        int write_full(int fd, char *buf, size_t n);
};