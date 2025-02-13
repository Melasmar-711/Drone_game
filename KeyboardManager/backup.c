#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "KeyBoard.h"
#include "logger.h"

bool is_paused = false;

int main(int argc, char *argv[]) {
    char* log_file = "../Logs/KeyBoard.log";

    // Collect PIDs of the Running Processes
    pid_t server_pid = -1;
    pid_t GameWindow = -1;
    pid_t DroneDynamicsManager = -1;
    pid_t Targets_Generator = -1;
    pid_t Obstacles_Generator = -1; // Fixed typo from "Obstcales_Generator"

    // Loop to get the PIDs from the arguments
    for (int i = 1; i < argc - 1; i++) { // Ensure i+1 is valid
        if (strcmp(argv[i], "server") == 0) {
            server_pid = (pid_t) strtol(argv[i + 1], NULL, 10);
        } else if (strcmp(argv[i], "GameWindow") == 0) {
            GameWindow = (pid_t) strtol(argv[i + 1], NULL, 10);
        } else if (strcmp(argv[i], "DroneDynamicsManager") == 0) {
            DroneDynamicsManager = (pid_t) strtol(argv[i + 1], NULL, 10);
        } else if (strcmp(argv[i], "Targets_Generator") == 0) {
            Targets_Generator = (pid_t) strtol(argv[i + 1], NULL, 10);
        } else if (strcmp(argv[i], "Obstacles_Generator") == 0) {
            Obstacles_Generator = (pid_t) strtol(argv[i + 1], NULL, 10);
        }
    }

    KeyboardInput input = {0, 0, 0}, prev_input = {0, 0, 0};

    log_message(log_file, INFO, "KeyboardManager started.");

    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server_%d", 0, O_WRONLY);

/*
  // Debugging: Print the arguments
    for (int i = 1; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    //Debugging: Print the PIDs
    printf("server_pid:%ds\n", server_pid);
    printf("GameWindow:%ds\n", GameWindow);
    printf("DroneDynamicsManager:%ds\n", DroneDynamicsManager);
    printf("Targets_Generator:%ds\n", Targets_Generator);
    printf("Obstacles_Generator:%ds\n", Obstacles_Generator);

*/

    init_ncurses();
    while (input.quit != 11) {

        clear(); // Clear the screen
        draw_keyboard_layout(&input);
        process_input(&input);

        // Sending pause/continue signal
        //sending pause or continue signal
        if (input.quit==Pause_or_Continue){   

            input.force_x=prev_input.force_x;
            input.force_y=prev_input.force_y;
            kill(server_pid, SIGUSR1);         
            kill(DroneDynamicsManager, SIGUSR1);
            usleep(10000);
            input.quit=0;
            continue;

        }


        //sending the stopping signal
        if (input.quit==stop){
        

            kill(server_pid, SIGINT);
            kill(DroneDynamicsManager, SIGINT);
            kill(Obstacles_Generator, SIGINT);
            //kill(Targets_Generator, SIGINT);
            kill(GameWindow, SIGINT);
            usleep(10000);
            exit(0);

        }

        //sending the reset signal
        if (input.quit==Re_set){

            input.force_x=0;
            input.force_y=0;

            kill(server_pid, SIGUSR2);
            kill(DroneDynamicsManager, SIGUSR2);
            kill(Obstacles_Generator, SIGUSR2);
            kill(Targets_Generator, SIGUSR2);
            kill(GameWindow, SIGUSR2);



            input.quit=0;
            
            usleep(10000);
            
        }

        write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
        prev_input = input;

        log_message(log_file, INFO, "KeyboardManager running.");
        usleep(DELAY);
    }

    close(fd_Keyboard_to_server);
    log_message(log_file, INFO, "KeyboardManager shutting down.");
    
    return 0;
}
