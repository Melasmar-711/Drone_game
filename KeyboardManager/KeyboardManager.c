#include "KeyBoard.h"
#include "logger.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define RESET_COOLDOWN 8  // 6-second cooldown for reset

bool is_paused = false;

int main() {
    char* log_file = "../Logs/KeyBoard.log";

    // Collect PIDs of the running processes
    pid_t server_pid = get_pidd("BlackBoardServer");
    pid_t GameWindow = get_pidd("GameWindow");
    pid_t DroneDynamicsManager = get_pidd("DroneDynamicsManager");
    pid_t Targets_Generator = get_pidd("Targets_Generator");
    pid_t Obstacles_Generator = get_pidd("Obstacle_Generator");

    KeyboardInput input = {0, 0, 0}, prev_input = {0, 0, 0}; // Initialize input structure
    time_t last_reset_time = 0; // Track last reset timestamp

    init_ncurses();
    log_message(log_file, INFO, "KeyboardManager started.");

    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server_%d", 0, O_WRONLY);

    while (input.quit != 11) {
        clear(); // Clear the screen
        draw_keyboard_layout(&input);
        process_input(&input);  // Process user input and update structure

        // Handle pause or continue signal
        if (input.quit == Pause_or_Continue) {
            input.force_x = prev_input.force_x;
            input.force_y = prev_input.force_y;
            kill(server_pid, SIGUSR1);
            kill(DroneDynamicsManager, SIGUSR1);
            usleep(10000);
            input.quit = 0;
            continue;
        }

        // Handle stop signal
        if (input.quit == stop) {
            input.force_x = prev_input.force_x;
            input.force_y = prev_input.force_y;
            kill(server_pid, SIGINT);
            kill(GameWindow, SIGINT);
            kill(DroneDynamicsManager, SIGINT);
            kill(Obstacles_Generator, SIGINT);
            kill(Targets_Generator, SIGINT);
            usleep(10000);
        }

        // Handle reset signal with cooldown
        if (input.quit == Re_set) {
            time_t current_time = time(NULL);
            if (difftime(current_time, last_reset_time) >= RESET_COOLDOWN) {
                input.force_x = 0;
                input.force_y = 0;
                kill(GameWindow, SIGUSR2);
                kill(server_pid, SIGUSR2);
                kill(DroneDynamicsManager, SIGUSR2);
                kill(Obstacles_Generator, SIGUSR2);
                kill(Targets_Generator, SIGUSR2);
                input.quit = 0;
                last_reset_time = current_time;  // Update last reset timestamp
                usleep(10000);
            } else {
                log_message(log_file, WARNING, "Reset attempted before cooldown expired.");
            }
        }

        // Send input to server
        write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
        prev_input = input;

        refresh();
        log_message(log_file, INFO, "KeyboardManager running.");
        usleep(DELAY); // Control frame rate
    }

    endwin();
    close(fd_Keyboard_to_server);
    log_message(log_file, INFO, "KeyboardManager shutting down.");
    
    return 0;
}
