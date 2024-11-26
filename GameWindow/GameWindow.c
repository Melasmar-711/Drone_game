#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_OBSTACLES 10
#define MAX_TARGETS 10
#define MAX_X 50
#define MAX_Y 30

typedef struct {
    int drone_x;
    int drone_y;
    int num_obstacles;
    int obstacles[MAX_OBSTACLES][2];
    int num_targets;
    int targets[MAX_TARGETS][2];
} ServerState;

#define DELAY 50000 // Delay in microseconds (controls frame rate)

// Function prototypes
void init_ncurses();
void draw_borders();
void draw_simulation(ServerState *prev_state, ServerState *current_state);

int main() {
    char *server_to_GameWindow = "/tmp/server_to_GameWindow";
    mkfifo(server_to_GameWindow, 0666);

    // Open the pipe to receive data from the server
    int fd_server_to_GameWindow = open(server_to_GameWindow, O_RDONLY | O_NONBLOCK);
    if (fd_server_to_GameWindow < 0) {
        perror("Failed to open pipe");
        return 1;
    }

    ServerState state;        // Current simulation state
    ServerState prev_state;   // Previous simulation state to track changes
    memset(&state, 0, sizeof(ServerState));
    memset(&prev_state, 0, sizeof(ServerState));

    // Initialize ncurses
    //init_ncurses();
    //draw_borders(); // Draw static borders once

    while (1) {
        // Read data from the server
        ssize_t bytes_read = read(fd_server_to_GameWindow, &state, sizeof(ServerState));

        printf("i read from server now%d\n",state.drone_x);
        fflush(stdout);

        if (bytes_read == sizeof(ServerState)) {
            // Only render changes when new data is received
            
            //draw_simulation(&prev_state, &state);
            refresh();
            // Update the previous state
            prev_state = state;
        }

        usleep(DELAY); // Control frame rate
    }

    //endwin(); // End ncurses mode
    close(fd_server_to_GameWindow); // Close the pipe
    return 0;
}

void init_ncurses() {
    initscr();
    noecho();
    curs_set(FALSE);
}

void draw_borders() {
    // Draw the static simulation boundary
    for (int i = 0; i < MAX_X; i++) {
        mvprintw(0, i, "-");
        mvprintw(MAX_Y, i, "-");
    }

    for (int i = 0; i <= MAX_Y; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, MAX_X, "|");
    }

    refresh(); // Refresh to display the borders
}

void draw_simulation(ServerState *prev_state, ServerState *current_state) {
    // Handle the drone position
    if (prev_state->drone_x != current_state->drone_x || prev_state->drone_y != current_state->drone_y) {
        // Erase old drone position
        mvprintw(prev_state->drone_y, prev_state->drone_x, " ");
        // Draw new drone position
        mvprintw(current_state->drone_y, current_state->drone_x, "D");
    }

    // Handle obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (i >= prev_state->num_obstacles && i < current_state->num_obstacles) {
            // New obstacle added
            mvprintw(current_state->obstacles[i][1], current_state->obstacles[i][0], "O");
        } else if (i < prev_state->num_obstacles && i >= current_state->num_obstacles) {
            // Obstacle removed
            mvprintw(prev_state->obstacles[i][1], prev_state->obstacles[i][0], " ");
        } else if (i < prev_state->num_obstacles && i < current_state->num_obstacles) {
            // Check if an obstacle moved
            if (prev_state->obstacles[i][0] != current_state->obstacles[i][0] ||
                prev_state->obstacles[i][1] != current_state->obstacles[i][1]) {
                mvprintw(prev_state->obstacles[i][1], prev_state->obstacles[i][0], " "); // Erase old position
                mvprintw(current_state->obstacles[i][1], current_state->obstacles[i][0], "O"); // Draw new position
            }
        }
    }

    // Handle targets
    for (int i = 0; i < MAX_TARGETS; i++) {
        if (i >= prev_state->num_targets && i < current_state->num_targets) {
            // New target added
            mvprintw(current_state->targets[i][1], current_state->targets[i][0], "T");
        } else if (i < prev_state->num_targets && i >= current_state->num_targets) {
            // Target removed
            mvprintw(prev_state->targets[i][1], prev_state->targets[i][0], " ");
        } else if (i < prev_state->num_targets && i < current_state->num_targets) {
            // Check if a target moved
            if (prev_state->targets[i][0] != current_state->targets[i][0] ||
                prev_state->targets[i][1] != current_state->targets[i][1]) {
                mvprintw(prev_state->targets[i][1], prev_state->targets[i][0], " "); // Erase old position
                mvprintw(current_state->targets[i][1], current_state->targets[i][0], "T"); // Draw new position
            }
        }
    }
}
