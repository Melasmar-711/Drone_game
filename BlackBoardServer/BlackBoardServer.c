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
#define MAX_X 50
#define MAX_Y 30
#define FRAME_RATE 20 // 20 FPS = 50ms per frame

typedef struct {
    int drone_x;
    int drone_y;
    int current_x_force;
    int current_y_force;
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
    if (mkfifo(fifo_name, 0666) == -1) {
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
    int fd_Dynamics_to_server = CREATE_AND_OPEN_FIFO("/tmp/DroneDynamics_to_server", O_RDONLY);
    int fd_server_to_Dynamics = CREATE_AND_OPEN_FIFO("/tmp/server_to_DroneDynamics", O_WRONLY);
    int fd_server_to_GameWindow = CREATE_AND_OPEN_FIFO("/tmp/server_to_GameWindow", O_WRONLY);
    int fd_Keyboard_to_server = CREATE_AND_OPEN_FIFO("/tmp/keyboardManager_to_server", O_RDONLY);



    // Server state
    ServerState state = {
        .drone_x = 10,
        .drone_y = 10,
        .current_x_force=0,
        .current_y_force=0,
        .num_obstacles = 3,
        .obstacles = {{5, 5}, {20, 7}, {30, 15}},
        .num_targets = 2,
        .targets = {{40, 3}, {25, 18}}
    };



    fd_set read_fds;
    int fds[] = {fd_Dynamics_to_server, fd_Keyboard_to_server};
    int max_fd = get_max_fd(fds, 2);



    struct timeval timeout = {0, 0};



    long last_frame_time = current_time_in_ms();
    KeyboardInput input;


    while (1) {


        long current_time = current_time_in_ms();
        if (current_time - last_frame_time < 1000 / FRAME_RATE) {
            usleep(1000); // Sleep for 1ms if we're ahead of the frame rate
            continue;
        }
        
        last_frame_time = current_time;

        FD_ZERO(&read_fds);
        FD_SET(fd_Dynamics_to_server, &read_fds);
        FD_SET(fd_Keyboard_to_server, &read_fds);

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);


        if (activity<=0){
            continue;
        }

        if (activity > 0) {
            // Handle input from KeyboardManager
            if (FD_ISSET(fd_Keyboard_to_server, &read_fds)) {
                
                ssize_t bytes_read = read(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
                if (bytes_read == sizeof(KeyboardInput)) {
                    if (input.quit) {
                        printf("Quit signal received. Shutting down.\n");
                        break;
                    }
                    printf("Received from Keyboard: Force X = %d, Force Y = %d\n", input.force_x, input.force_y);
                    // Update drone position based on forces (simplified for now)



                }
            }

            // Handle input from DroneDynamics (placeholder for now)
            /*if (FD_ISSET(fd_Dynamics_to_server, &read_fds)) {
                char buffer[256];
                ssize_t bytes_read = read(fd_Dynamics_to_server, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Received from DroneDynamics: %s\n", buffer);
                }
            }*/
        }



        /*
        state.drone_x += input.force_x;
        state.drone_y += input.force_y;

        // Enforce geofence boundaries
        if (state.drone_x <= 0) state.drone_x = 1;
        if (state.drone_x >= MAX_X) state.drone_x = MAX_X - 1;
        if (state.drone_y <= 0) state.drone_y = 1;
        if (state.drone_y >= MAX_Y) state.drone_y = MAX_Y - 1;  */

        // Send updated state to GameWindow
        write(fd_server_to_GameWindow, &state, sizeof(ServerState));


        write(fd_server_to_Dynamics, &state, sizeof(ServerState));
 
    }

    // Close pipes
    close(fd_Dynamics_to_server);
    close(fd_server_to_Dynamics);
    close(fd_server_to_GameWindow);
    close(fd_Keyboard_to_server);

    return 0;
}
