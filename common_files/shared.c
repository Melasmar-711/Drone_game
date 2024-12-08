#include"shared.h"
#include<errno.h>




int create_and_open_fifo(const char *template, int identifier, int flags) {
    char fifo_name[256];
    snprintf(fifo_name, sizeof(fifo_name), template, identifier);

    if (mkfifo(fifo_name, 0666) < 0 && errno != EEXIST) {
        perror("Failed to create FIFO");
        exit(1);
    }

    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    return fd;
}



ServerState initialize_server_state() {
    return (ServerState){
        .drone_x = 10,
        .drone_y = 7,
        .input_x_force = 0,
        .input_y_force = 0,
        .resultant_force_x = 0,
        .resultant_force_y = 0,
        .velocity_x = 0,
        .velocity_y = 0,
        .num_obstacles = MAX_OBSTACLES,
        .num_targets = MAX_TARGETS,
    };
}

