
#include "server_functions.h"
#include"parameters.h"

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
