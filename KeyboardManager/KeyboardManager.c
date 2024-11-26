#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Define the structure in a shared header file (e.g., `shared.h`)
typedef struct {
    int force_x; // Force in the x-direction
    int force_y; // Force in the y-direction
    int quit;    // Flag to indicate if the user wants to quit
} KeyboardInput;

#define DELAY 50000 // Delay in microseconds (adjust frame rate)

void init_ncurses();
void process_input(KeyboardInput *input);

int main() {
    char *Keyboard_to_server = "/tmp/keyboardManager_to_server";
    mkfifo(Keyboard_to_server, 0666);

    printf("i am here");
    fflush(stdout);

    int fd_Keyboard_to_server = open(Keyboard_to_server, O_WRONLY);
    if (fd_Keyboard_to_server < 0) {
        perror("Failed to open pipe");
        return 1;
    }

    KeyboardInput input = {0, 0, 0};        // Current state
    KeyboardInput prev_input = {0, 0, 0};  // Previous state to track changes

    init_ncurses();

    while (!input.quit) {
        clear(); // Clear the screen

        // Display current forces
        mvprintw(5, 10, "Keyboard Manager");
        mvprintw(7, 10, "Force X: %d", input.force_x);
        mvprintw(8, 10, "Force Y: %d", input.force_y);
        mvprintw(10, 10, "Press Arrow Keys to Change Force");
        mvprintw(11, 10, "Press 'q' to Quit");

        refresh();

        // Process user input and update the structure
        process_input(&input);

        // Check if forces have changed
        if (input.force_x != prev_input.force_x || input.force_y != prev_input.force_y || input.quit != prev_input.quit) {
            // Send the structure to the server
            write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));

            // Update the previous state
            prev_input = input;
        }

        usleep(DELAY);
    }

    endwin();
    close(fd_Keyboard_to_server);
    return 0;
}

void init_ncurses() {
    initscr();            // Initialize the screen
    noecho();             // Don't echo user input
    curs_set(FALSE);      // Hide the cursor
    nodelay(stdscr, TRUE); // Non-blocking input
    keypad(stdscr, TRUE);  // Enable arrow keys
}

void process_input(KeyboardInput *input) {
    int ch = getch(); // Get user input (non-blocking)

    switch (ch) {
        case 'w':
            input->force_y--; // Increase upward force
            break;
        case 'x':
            input->force_y++; // Decrease downward force
            break;
        case 'a':
            input->force_x--; // Decrease rightward force
            break;
        case 'd':
            input->force_x++; // Increase rightward force
            break;
        case 's':
            input->force_x=0; // Increase rightward force
            input->force_y=0;
            break;
        case 'e':
            input->force_x+=1;//0.70710678118; // Increase rightward force
            input->force_y-=1;//0.70710678118; // Increase rightward force            
            break;
        case 'q':
            input->force_x-=1;//0.70710678118; // Increase rightward force
            input->force_y-=1;//0.70710678118; // Increase rightward force            
            break;                    
        case 'z':
            input->force_x-=1;//0.70710678118; // Increase rightward force
            input->force_y+=1;//0.70710678118; // Increase rightward force            
            break;
        case 'c':
            input->force_x+=1;//0.70710678118; // Increase rightward force
            input->force_y+=1;//0.70710678118; // Increase rightward force            
            break;

        default:
            // No action for other keys
            break;
    }
}
