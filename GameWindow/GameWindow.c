#include "Window.h"
#include "sig_handle.h"
#include "logger.h"
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define FIFO_PATH "/tmp/server_to_GameWindow_%d"

int MAX_X = 100;
int MAX_Y = 30;

int main() {
    char* log_file = "../Logs/GameWindow.log";

    // Load configuration values
    get_int_from_json("../Game_Config.json", "MAX_X", &MAX_X);
    get_int_from_json("../Game_Config.json", "MAX_Y", &MAX_Y);

    int n_obstacles, n_targets;
    get_int_from_json("../Game_Config.json", "num_of_obstacles", &n_obstacles);
    get_int_from_json("../Game_Config.json", "num_of_targets", &n_targets);

    // Signal handling
    signal(SIGUSR1, handle_pause_signal);
    signal(SIGUSR2, handle_reset_signal);
    signal(SIGINT, handle_stop_signal);

    log_message(log_file, INFO, "GameWindow started successfully.");

    int fifo_id = 0;
    int fd_server_to_GameWindow;
    ServerState state, prev_state;
    bool just_got_reset = false;

    while (!stop) {
        if (reset) {
            just_got_reset = true;
            clear();
            reset = false;
            fifo_id++;

            log_message(log_file, INFO, "GameWindow reset.");
            usleep(10000);

            // Reload game configuration after reset
            get_int_from_json("../Game_Config.json", "MAX_X", &MAX_X);
            get_int_from_json("../Game_Config.json", "MAX_Y", &MAX_Y);
            get_int_from_json("../Game_Config.json", "num_of_obstacles", &n_obstacles);
            get_int_from_json("../Game_Config.json", "num_of_targets", &n_targets);
        }

        // Create and open FIFO for communication
        char fifo_name[50];
        fd_server_to_GameWindow = create_and_open_fifo(FIFO_PATH, fifo_id, O_RDONLY | O_NONBLOCK);
        if (fd_server_to_GameWindow == -1) {
            perror("Error opening FIFO");
            exit(EXIT_FAILURE);
        }

        // Initialize game state
        state = initialize_server_state(n_obstacles, n_targets);
        memset(&prev_state, 0, sizeof(ServerState));
        prev_state = state;  // Copy initial state

        // Initialize ncurses UI
        init_ncurses();
        draw_borders(MAX_X, MAX_Y);
        refresh();

        int target_active_flags[state.num_obstacles];
        memset(target_active_flags, 0, sizeof(target_active_flags));

        while (!stop) {
            if (is_paused) {
                log_message(log_file, INFO, "GameWindow paused.");
                usleep(100000);
                continue;
            }

            // Read server state from FIFO
            ssize_t bytes_read = read(fd_server_to_GameWindow, &state, sizeof(ServerState));

            if (bytes_read != sizeof(ServerState)) {
                log_message(log_file, ERROR, "Failed to read the correct number of bytes from server.");
                state = prev_state;  // Keep previous state
                continue;
            }

            // Draw updated simulation
            draw_simulation(&prev_state, &state, target_active_flags,just_got_reset);
            refresh();

            prev_state = state;  // Update previous state
            log_message(log_file, INFO, "GameWindow running.");

            if (reset) {
                usleep(1000000);
                close(fd_server_to_GameWindow);
                memset(target_active_flags, 0, sizeof(target_active_flags));

                break;  // Restart loop after reset
            }

            usleep(DELAY);
        }

        // Cleanup
        endwin();
        close(fd_server_to_GameWindow);
    }

    log_message(log_file, INFO, "GameWindow shutting down.");
    return 0;
}
