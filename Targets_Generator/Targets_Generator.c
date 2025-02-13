#include "Generator_functions.h"
#include "sig_handle.h"
#include "logger.h"
#include <fcntl.h>      // For FIFO flags
#include <stdio.h>      // For printf, perror
#include <stdlib.h>     // For rand, exit
#include <unistd.h>     // For close, usleep
#include <signal.h>     // For signal handling
#include <time.h>       // For srand, time

#define FIFO_PATH "/tmp/target_generator_to_server_%d"

int main() {
    char* log_file = "../Logs/TargetsGenerator.log";
    signal(SIGUSR2, handle_reset_signal);
    signal(SIGINT, handle_stop_signal);

    log_message(log_file, INFO, "TargetsGenerator started successfully.");
    printf("Starting TargetsGenerator...\n");

    int fifo_id = 0;
    int fps_value, MAX_X, MAX_Y, n_targets;
    char fifo_name[50];

    while (!stop) {
        // Load configuration values
        get_int_from_json("../Game_Config.json", "MAX_X", &MAX_X);
        get_int_from_json("../Game_Config.json", "MAX_Y", &MAX_Y);
        get_int_from_json("../Game_Config.json", "FPS", &fps_value);
        get_int_from_json("../Game_Config.json", "num_of_targets", &n_targets);

        printf("Configuration: MAX_X=%d, MAX_Y=%d, FPS=%d, num_of_targets=%d\n", MAX_X, MAX_Y, fps_value, n_targets);

        // Seed random number generator (keeping your logic intact)
        srand(time(NULL) + 1);

        // Generate FIFO name and open it
        int fd_target_generator_to_server = create_and_open_fifo(FIFO_PATH, fifo_id, O_WRONLY);
        if (fd_target_generator_to_server == -1) {
            perror("Error opening FIFO");
            exit(EXIT_FAILURE);
        }

        
            int targets[n_targets][2];

            // Generate targets using your original random value logic
            for (int i = 0; i < n_targets; i++) {
                targets[i][0] = (rand() % ((MAX_X - 2) / 2)) * 2 + 2;  // X coordinate (even)
                targets[i][1] = (rand() % ((MAX_Y - 2) / 2)) * 2 + 2;  // Y coordinate (even)
            }

            // Send targets to the server
            ssize_t bytes_written = write(fd_target_generator_to_server, targets, sizeof(targets));
            if (bytes_written == -1) {
                perror("Error writing to FIFO");
                log_message(log_file, ERROR, "Failed to write to FIFO.");
                break;
            }

            printf("Generated and sent %d targets.\n", n_targets);
            log_message(log_file, INFO, "TargetsGenerator sent targets.");


        while (!reset && !stop) {  

                        usleep(1000000 / fps_value);
                        log_message(log_file, INFO, "TargetsGenerator running.");

  
        }

        close(fd_target_generator_to_server);

        if (reset) {
            log_message(log_file, INFO, "TargetsGenerator resetting...");
            reset = false;
            fifo_id++;
            usleep(100000);
        }
    }

    log_message(log_file, INFO, "TargetsGenerator shutting down.");
    return 0;
}
