#include "server_functions.h"

int create_and_open_fifo(const char *fifo_name, int flags) {
    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }
    return fd;
}

long current_time_in_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int get_max_fd(int fds[], int num_fds) {
    int max_fd = fds[0];
    for (int i = 1; i < num_fds; i++) {
        if (fds[i] > max_fd) {
            max_fd = fds[i];
        }
    }
    return max_fd;
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

void handle_keyboard_input(int fd, KeyboardInput *input, KeyboardInput *prev_input, ServerState *state) {
    ssize_t bytes_read = read(fd, input, sizeof(KeyboardInput));
    if (bytes_read == sizeof(KeyboardInput)) {
        if (input->quit) {
            printf("Quit signal received. Shutting down.\n");
            exit(0);
        }
        if (memcmp(input, prev_input, sizeof(KeyboardInput))) {
            state->input_x_force = input->force_x;
            state->input_y_force = input->force_y;
            *prev_input = *input;
        }
    }
}

void send_state_to_dynamics(int fd, KeyboardInput *input, KeyboardInput *prev_input, ServerState *state) {
    if (memcmp(input, prev_input, sizeof(KeyboardInput))) {
        write(fd, state, sizeof(ServerState));
    }
}

void handle_dynamics_input(int fd, ServerState *state) {
    ssize_t bytes_read = read(fd, state, sizeof(ServerState));
    if (bytes_read == sizeof(ServerState)) {
        printf("Updated state received from DroneDynamics: %f\n", state->drone_x);
    }
}

void close_pipes(int fd1, int fd2, int fd3, int fd4) {
    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
}

