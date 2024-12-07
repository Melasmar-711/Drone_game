#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <math.h>

#include"sig_handle.h"



#define MAX_OBSTACLES 10
#define MAX_TARGETS 10

#define Target_catched 200
#define MAX_X 100
#define MAX_Y 30

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
void draw_simulation(ServerState *prev_state, ServerState *current_state,int *flags);







int main() {
    
    
    signal(SIGUSR1, handle_pause_signal);



    fd_set read_fds;
    struct timeval timeout = {0, 0};


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
    //draw_simulation(&prev_state,&state,target_active_flags);

    refresh();


    while (1) {


        if (is_paused) {
            usleep(100000); // Sleep while paused to reduce CPU usage
            continue;
        }



        ssize_t bytes_read = read(fd_server_to_GameWindow, &state, sizeof(ServerState));
        
    
        draw_simulation(&prev_state,&state,target_active_flags);

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
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);  // Drone
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Targets
    init_pair(3, COLOR_RED, COLOR_BLACK);   // Obstacles
}




void draw_borders() {

    // Draw the static simulation boundary
    attron(COLOR_PAIR(3));
    for (int i = 0; i < MAX_X; i++) {
        mvprintw(0, i, "-");
        mvprintw(MAX_Y, i, "-");
    }

    for (int i = 0; i <= MAX_Y; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, MAX_X, "|");
    }

    refresh(); // Refresh to display the borders
    attroff(COLOR_PAIR(3)); // Stop using the obstacle color
}





void draw_simulation(ServerState *prev_state, ServerState *current_state, int *flags) {
    // Handle obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (current_state->obstacles[i][0] == 0 && current_state->obstacles[i][1] == 0) {
            // Use previous state values if current values are zero
            current_state->obstacles[i][0] = prev_state->obstacles[i][0];
            current_state->obstacles[i][1] = prev_state->obstacles[i][1];
        }

        // Erase old obstacle position before drawing a new one
        if (prev_state->obstacles[i][0] != current_state->obstacles[i][0] || prev_state->obstacles[i][1] != current_state->obstacles[i][1]) {
            mvprintw(prev_state->obstacles[i][1], prev_state->obstacles[i][0], " ");
        }

        attron(COLOR_PAIR(3));
        // Draw the new obstacle position
        mvprintw(current_state->obstacles[i][1], current_state->obstacles[i][0], "O");
        attroff(COLOR_PAIR(3));
    }

    // Handle targets
    for (int i = 0; i < current_state->num_targets; i++) {
        static int prev_flags[MAX_TARGETS] = {0};
        static int score = 0;

        // Ensure target positions are updated correctly
        if (current_state->targets[i][0] == 0 && current_state->targets[i][1] == 0) {
            // Use previous state values if current values are zero
            current_state->targets[i][0] = prev_state->targets[i][0];
            current_state->targets[i][1] = prev_state->targets[i][1];
        }

        // Calculate distance from drone to target
        int dx = current_state->drone_x - current_state->targets[i][0];
        int dy = current_state->drone_y - current_state->targets[i][1];
        double distance = sqrt(dx * dx + dy * dy);

        // Check if the target is "taken"
        if (distance < 0.2 && prev_flags[i] != 1) {
            flags[i] = 1;  // Target is taken now
            prev_flags[i] = 1;  // Update flag to indicate target is taken
            score++;
        }

        attron(COLOR_PAIR(2));

        // Only draw targets that are not "taken"
        if (flags[i] == 0) {
            mvprintw(current_state->targets[i][1], current_state->targets[i][0], "T");
        } else {
            // Erase target if it is taken
            mvprintw(current_state->targets[i][1], current_state->targets[i][0], " ");
        }

        // Print score
        mvprintw(35, 40, "score : ");
        mvprintw(35, 48, "%d", score);

        attroff(COLOR_PAIR(2));
    }
    // Handle the drone position
    if (prev_state->drone_x != current_state->drone_x || prev_state->drone_y != current_state->drone_y) {
        // Erase old drone position
        attron(COLOR_PAIR(1));
        mvprintw((int) prev_state->drone_y, (int) prev_state->drone_x, " ");
        // Draw new drone position
        mvprintw((int) current_state->drone_y, (int) current_state->drone_x, "+");
        attroff(COLOR_PAIR(1));
    }
}
