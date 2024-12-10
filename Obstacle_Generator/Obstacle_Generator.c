#include "Generator_functions.h"
#include "sig_handle.h"
#include"logger.h"


int main() {

    char* log_file = "../Logs/ObstacleGenerator.log";
    signal(SIGUSR1, handle_pause_signal);
    signal(SIGUSR2, handle_reset_signal);
    signal(SIGINT, handle_stop_signal);
    int fifo_id=0;

    log_message(log_file, INFO, "ObstacleGenerator started successfully.");

start:

    if(reset){

        log_message(log_file, INFO, "ObstacleGenerator reset.");
        fifo_id++;
        reset=false;
        usleep(10000);

    }

    reset=false;  

    // Seed random number generator
    srand(time(NULL) + 2);

    // Create FIFO
    int fd_obstacle_generator_to_server = create_and_open_fifo("/tmp/obstacle_generator_to_server_%d",fifo_id, O_WRONLY);
    int num_obstacles = MAX_OBSTACLES; 
    int obstacles[MAX_OBSTACLES][2];


    while (!stop) {


        if (is_paused) {
            usleep(100000); // Sleep while paused to reduce CPU usage
            continue;
        }

        

        // Generate random obstacles within specified boundaries
        for (int i = 0; i < num_obstacles; i++) {
            obstacles[i][0] = rand() % (MAX_X - 2) + 1;  // X coordinate
            obstacles[i][1] = rand() % (MAX_Y - 2) + 1;  // Y coordinate
        }


        // Send the obstacles array to the server
        ssize_t bytes_written = write(fd_obstacle_generator_to_server, obstacles, sizeof(obstacles));
        if (bytes_written == -1) {
            perror("Error writing to FIFO");
            close(fd_obstacle_generator_to_server);
            return 1;
        }

        printf("Generated and sent %d obstacles.\n", num_obstacles);

        
        // Wait before generating new obstacles
        for(int i=0;i<20;i++){

            if(reset){
                close(fd_obstacle_generator_to_server);
                goto start;
            } 
            else{  
                log_message(log_file, INFO, "ObstacleGenerator running.");
                usleep(500000);
            }

        }

    }

    close(fd_obstacle_generator_to_server);
    return 0;
}
