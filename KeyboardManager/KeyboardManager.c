#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>
#include<signal.h>

// Define the structure in a shared header file (e.g., `shared.h`)
typedef struct {
    int force_x; // Force in the x-direction
    int force_y; // Force in the y-direction
    int quit;    // Flag to indicate if the user wants to quit
} KeyboardInput;

#define DELAY 500000 // Delay in microseconds

#define Pause 10
#define stop  11
#define Continue 12


static inline int create_and_open_fifo(const char *fifo_name, int flags) {

    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    return fd;
}

pid_t get_pid(const char *program_name) {
    char line[256];
    // Construct the command to get the PID of the process
    char command[256];
    snprintf(command, sizeof(command), "pidof %s", program_name);

    // Open the command for reading
    FILE *cmd = popen(command, "r");
    if (cmd == NULL) {
        perror("Failed to run pidof");
        return -1;
    }

    // Read the output (the PID) from the command
    if (fgets(line, 256, cmd) != NULL) {
        // Convert the string to pid_t (unsigned long) and return the PID
        pid_t pid = strtoul(line, NULL, 10);
        pclose(cmd);
        return pid;
    } else {
        // If no output (process not found), close and return -1
        pclose(cmd);
        return -1;
    }
}



void init_ncurses();
void process_input(KeyboardInput *input);
void draw_keyboard_layout(KeyboardInput *input) ;

int main() {

    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server", O_WRONLY);

    pid_t server_pid=get_pid("BlackBoardServer");
    pid_t GameWindow=get_pid("GameWindow");
    


    KeyboardInput input = {0, 0, 0}, prev_input = {0, 0, 0};
    init_ncurses();

    while (input.quit!=11) {
        clear(); // Clear the screen

        draw_keyboard_layout(&input);

        // Process user input and update the structure
        
        process_input(&input);


        if (input.quit==Pause || input.quit==Continue){
            
            input.force_x=prev_input.force_x;
            input.force_y=prev_input.force_y;
            
            kill(server_pid, SIGUSR1);

            usleep(10000);
            input.quit=0;
            continue;
        }
        

        // Check if forces have changed or quit signal is sent
        //if (memcmp(&input, &prev_input, sizeof(KeyboardInput)) ) {
        write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
        prev_input = input;
        
        //}
        

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
    //timeout(100);
    keypad(stdscr, TRUE);
}

void process_input(KeyboardInput *input) {
    int ch = getch(); 

    switch (ch) {
        case 'w': input->force_y-=1; break;
        case 's': input->force_x = 0; input->force_y = 0; break;
        case 'a': input->force_x-=1; break;
        case 'd': input->force_x+=1; break;
        case 'x': input->force_y+=1; break;
        case 'q': input->force_x -= 1; input->force_y-=1; break;
        case 'e': input->force_x += 1; input->force_y -=1; break;
        case 'z': input->force_x-=1; input->force_y += 1; break;
        case 'c': input->force_x+= 1; input->force_y+= 1; break;
        case 'o': input->quit = 1; break; // Quit
        case 'p': input->quit=10;break;
        case ']': input->quit=Continue;

        //case ERR :input->force_x = 0; input->force_y = 0; break;
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
