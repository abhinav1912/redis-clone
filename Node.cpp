#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

class Node {
    private:
        void log_error_and_abort(const char *msg) {
            int err = errno;
            fprintf(stderr, "[%d] %s\n", err, msg);
            abort();
        }

        int read_full(int fd, char *buf, size_t n) {
            // TODO: complete implementation
            return 0;
        }

        int write_full(int fd, char *buf, size_t n) {
            // TODO: complete implementation
            return 0;
        }
};