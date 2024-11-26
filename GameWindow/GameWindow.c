#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>


#define MAX_OBSTACLES 10
#define MAX_TARGETS 10
#define MAX_X 50
#define MAX_Y 20

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


static inline int create_and_open_fifo(const char *fifo_name, int flags) {

    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    return fd;
}



#define DELAY 1000 // Delay in microseconds (controls frame rate)

// Function prototypes
void init_ncurses();
void draw_borders();
void draw_simulation(ServerState *prev_state, ServerState *current_state);

int main() {
    
    
    fd_set read_fds;
    struct timeval timeout = {0, 0};


    int fd_server_to_GameWindow = create_and_open_fifo("/tmp/server_to_GameWindow", O_RDONLY | O_NONBLOCK);
    if (fd_server_to_GameWindow < 0) {
        perror("Failed to open pipe");
        return 1;
    }



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


    ServerState prev_state={0};
    init_ncurses();
    draw_borders();
    draw_simulation(&prev_state,&state);

    refresh();


    while (1) {


        FD_ZERO(&read_fds);
        FD_SET(fd_server_to_GameWindow, &read_fds);

        int activity = select(fd_server_to_GameWindow + 1, &read_fds, NULL, NULL, &timeout);

        if (activity>0){


            ssize_t bytes_read = read(fd_server_to_GameWindow, &state, sizeof(ServerState));
        }

        
        draw_simulation(&prev_state,&state);
        refresh();
        prev_state=state;

        
        
        
        usleep(DELAY);
    }

    endwin();
    close(fd_server_to_GameWindow);
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

        mvprintw((int) prev_state->drone_y, (int)prev_state->drone_x, " ");
        // Draw new drone position
        mvprintw((int)current_state->drone_y, (int)current_state->drone_x, "D");
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
