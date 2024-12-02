#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>

#define MAX_OBSTACLES 10
#define MAX_TARGETS 10
#define Target_catched 200


#define MAX_X 100
#define MAX_Y 30
#define FRAME_RATE 30

typedef struct {
    float drone_x;
    float drone_y;
    int input_x_force;
    int input_y_force;
    float resultant_force_x;
    float resultant_force_y;
    float velocity_x;
    float velocity_y;
    int num_obstacles;
    int obstacles[MAX_OBSTACLES][2];
    int num_targets;
    int targets[MAX_TARGETS][2];
} ServerState;


// Define the structure in a shared header file (e.g., `shared.h`)
typedef struct {
    int force_x; // Force in the x-direction
    int force_y; // Force in the y-direction
    int quit;    // Flag to indicate if the user wants to quit
} KeyboardInput;

static inline int create_and_open_fifo(const char *fifo_name, int flags) {

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
    gettimeofday(&tv, NULL); // Get current time
    return tv.tv_sec * 1000 + tv.tv_usec / 1000; // Convert to milliseconds
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

int main() {

    
    // Create FIFOs
    int fd_Dynamics_to_server = create_and_open_fifo("/tmp/DroneDynamics_to_server", O_RDONLY|O_NONBLOCK);
    int fd_server_to_Dynamics = create_and_open_fifo("/tmp/server_to_DroneDynamics", O_WRONLY);
    int fd_server_to_GameWindow = create_and_open_fifo("/tmp/server_to_GameWindow", O_WRONLY);
    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server", O_RDONLY|O_NONBLOCK);


    // Server state
    ServerState state = {
        .drone_x = 10,
        .drone_y = 7,
        .input_x_force = 0,
        .input_y_force = 0,
        .resultant_force_x = 0,
        .resultant_force_y = 0,
        .velocity_x = 0,
        .velocity_y = 0,
        .num_obstacles = 3,
        .obstacles = {{5, 5}, {20, 7}, {30, 15}},
        .num_targets = 2,
        .targets = {{40, 3}, {25, 18}}
    };

    fd_set read_fds;
    int fds[] = {fd_Dynamics_to_server, fd_Keyboard_to_server};
    int max_rfd = get_max_fd(fds, 2);


    fd_set write_fds;
    int fds_w[] = {fd_server_to_GameWindow, fd_server_to_Dynamics};
    int max_wfd = get_max_fd(fds, 2);


    int max_fd = max_rfd>max_wfd? max_rfd:max_wfd;

    struct timeval timeout = {0, 0};
    long last_frame_time = current_time_in_ms();
    KeyboardInput prev_input={0};
    KeyboardInput input={0};

    while (1) {

        long current_time = current_time_in_ms();
        if (current_time - last_frame_time < 1000 / FRAME_RATE) {
            //printf("sleeping for sometime\n");
            fflush(stdout);
            usleep(1000); // Sleep for 1ms if we're ahead of the frame rate
            continue;
        }
        last_frame_time = current_time;



        FD_ZERO(&read_fds);
        FD_SET(fd_Dynamics_to_server, &read_fds);
        FD_SET(fd_Keyboard_to_server, &read_fds);


        FD_ZERO(&write_fds);
        FD_SET(fd_server_to_GameWindow, &write_fds);
        FD_SET(fd_server_to_Dynamics, &write_fds);

        int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);



        if (activity > 0) {





            // Handle input from KeyboardManager
            if (FD_ISSET(fd_Keyboard_to_server, &read_fds)) {
                
                ssize_t bytes_read = read(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
                
                if (bytes_read == sizeof(KeyboardInput)) {
                    if (input.quit) {
                        printf("Quit signal received. Shutting down.\n");
                        break;
                    }

                if (memcmp(&input, &prev_input, sizeof(KeyboardInput)) ) {
                    state.input_x_force = input.force_x;
                    state.input_y_force = input.force_y;
                    //printf("Received from Keyboard: Force X = %d, Force Y = %d\n", input.force_x, input.force_y);

                        }
                    
                }
            }





        // Send updated state to GameWindow
            if (FD_ISSET(fd_server_to_GameWindow, &write_fds)) {
                
            write(fd_server_to_GameWindow, &state, sizeof(ServerState));

            }





        if (FD_ISSET(fd_server_to_Dynamics, &write_fds)) {
                

            if (prev_input.force_x!= input.force_x || prev_input.force_y!= input.force_y)
            {

            printf("i am sending to the dynamics now %d %d \n",state.input_x_force,state.input_y_force);

            write(fd_server_to_Dynamics, &state, sizeof(ServerState));
            prev_input=input;
            }

        }


        // Handle input from DroneDynamics
            if (FD_ISSET(fd_Dynamics_to_server, &read_fds)) {
                ssize_t bytes_read = read(fd_Dynamics_to_server, &state, sizeof(ServerState));
                if (bytes_read == sizeof(ServerState)) {
                    // Enforce geofence boundaries

                    printf("Updated state received from DroneDynamics %f\n ",state.drone_x);
                }


            }




        


    }
    }

    // Close pipes
    close(fd_Dynamics_to_server);
    close(fd_server_to_Dynamics);
    close(fd_server_to_GameWindow);
    close(fd_Keyboard_to_server);

    return 0;
}
