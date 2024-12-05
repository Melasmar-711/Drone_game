#include "Generator_functions.h"

int create_and_open_fifo(const char *fifo_name, int flags) {
    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }
    return fd;
}
