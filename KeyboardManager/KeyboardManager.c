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


static inline int create_and_open_fifo(const char *fifo_name, int flags) {

    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    return fd;
}


#define DELAY 50000 // Delay in microseconds

void init_ncurses();
void process_input(KeyboardInput *input);
void draw_keyboard_layout(KeyboardInput *input);

int main() {

    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server", O_WRONLY);


    KeyboardInput input = {0, 0, 0}, prev_input = {0, 0, 0};
    init_ncurses();

    while (!input.quit) {
        clear(); // Clear the screen

        draw_keyboard_layout(&input);

        // Process user input and update the structure
        process_input(&input);

        // Check if forces have changed or quit signal is sent
        if (memcmp(&input, &prev_input, sizeof(KeyboardInput)) != 0) {
            write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
            prev_input = input;
        }

        refresh();
        usleep(DELAY); // Control the frame rate
    }

    endwin();
    close(fd_Keyboard_to_server);
    return 0;
}

void init_ncurses() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

void process_input(KeyboardInput *input) {
    int ch = getch(); // Get user input (non-blocking)

    switch (ch) {
        case 'w': input->force_y--; break;
        case 's': input->force_x = 0; input->force_y = 0; break;
        case 'a': input->force_x--; break;
        case 'd': input->force_x++; break;
        case 'x': input->force_y++; break;
        case 'q': input->force_x -= 1; input->force_y -= 1; break;
        case 'e': input->force_x += 1; input->force_y -= 1; break;
        case 'z': input->force_x -= 1; input->force_y += 1; break;
        case 'c': input->force_x += 1; input->force_y += 1; break;
        case 'o': input->quit = 1; break; // Quit
        default: break;
    }
}

void draw_keyboard_layout(KeyboardInput *input) {
    mvprintw(5, 10, "Keyboard Layout:");
    mvprintw(7, 10, "  q | w | e  ");
    mvprintw(8, 10, "  -----------  ");
    mvprintw(9, 10, "  a | s | d  ");
    mvprintw(10, 10, "  -----------  ");
    mvprintw(11, 10, "  z | x | c  ");

    mvprintw(13, 10, "Force X: %d", input->force_x);
    mvprintw(14, 10, "Force Y: %d", input->force_y);

    mvprintw(16, 10, "Controls:");
    mvprintw(17, 10, "'w': Move Up, 'a': Move Left, 's': Stop, 'd': Move Right");
    mvprintw(18, 10, "'q': Up-Left, 'e': Up-Right, 'z': Down-Left, 'c': Down-Right");
    mvprintw(19, 10, "'o': Quit");
}
