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


void unlink_fifo(const char *template, int identifier) {
    char fifo_name[256];
    snprintf(fifo_name, sizeof(fifo_name), template, identifier);

    if (unlink(fifo_name) < 0) {
        perror("Failed to unlink FIFO");
    } else {
        printf("FIFO %s successfully unlinked.\n", fifo_name);
    }
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



pid_t get_pidd(const char *program_name) {
    char line[256];
    // Construct the command to get the PID of the process
    char command[256];
    snprintf(command, sizeof(command), "pidof %s", program_name);

    // Open the command for reading
    FILE *cmd = popen(command, "r");
    if (cmd == NULL) {
        perror("Failed to run pidof");
        return -1;
    }

    // Read the output (the PID) from the command
    if (fgets(line, 256, cmd) != NULL) {
        // Convert the string to pid_t (unsigned long) and return the PID
        pid_t pid = strtoul(line, NULL, 10);
        pclose(cmd);
        return pid;
    } else {
        // If no output (process not found), close and return -1
        pclose(cmd);
        return -1;
    }
}


