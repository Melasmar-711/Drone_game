#include"Window.h"
#include"sig_handle.h"


int main() {
    
    
    signal(SIGUSR1, handle_pause_signal);
    signal(SIGINT, handle_stop_signal);
    signal(SIGUSR2, handle_reset_signal);








    int fd_server_to_GameWindow = create_and_open_fifo("/tmp/server_to_GameWindow", O_RDONLY );

    
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
        .num_obstacles = MAX_OBSTACLES,
        .num_targets = MAX_TARGETS,
    };


    int target_active_flags[state.num_obstacles];
    memset(target_active_flags, 0, sizeof(target_active_flags)); // Set all bytes of arr to 0



    ServerState prev_state={0};
    init_ncurses();
    draw_borders();

    refresh();


    while (!stop) {



        reset=false;


        if (is_paused) {
            usleep(100000); // Sleep while paused to reduce CPU usage
            continue;
        }



        ssize_t bytes_read = read(fd_server_to_GameWindow, &state, sizeof(ServerState));

        
    
        draw_simulation(&prev_state,&state,target_active_flags);

        refresh();

        prev_state=state; 




        usleep(DELAY);

        if(reset){
            
            memset(target_active_flags, 0, sizeof(target_active_flags)); // Set all bytes of arr to 0
            usleep(100000);
        }



    }






    endwin();
    close(fd_server_to_GameWindow);
    return 0;
}




