
#include"KeyBoard.h"



bool is_paused=false;

int main() {


    
    int fd_Keyboard_to_server = create_and_open_fifo("/tmp/keyboardManager_to_server", O_WRONLY); 



    //Collect PIDs of the Running the processes 
    pid_t server_pid=get_pidd("BlackBoardServer");
    pid_t GameWindow=get_pidd("GameWindow");
    pid_t DroneDynamicsManager=get_pidd("DroneDynamicsManager");
    pid_t Targets_Generator=get_pidd("Targets_Generator");
    pid_t Obstcales_Generator=get_pidd("Obstacle_Generator");




    KeyboardInput input = {0, 0, 0}, prev_input = {0, 0, 0}; // initialize the structure of inputs


    init_ncurses();


    
    while (input.quit!=11) {

        clear(); // Clear the screen

        draw_keyboard_layout(&input);

                
        process_input(&input);  // Process user input and update the structure


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
            
            input.force_x=prev_input.force_x;
            input.force_y=prev_input.force_y;            
            kill(server_pid, SIGINT);
            kill(GameWindow, SIGINT);
            kill(DroneDynamicsManager, SIGINT);
            kill(Obstcales_Generator, SIGINT);
            kill(Targets_Generator, SIGINT);
            usleep(10000);
        }

        //sending the reset signal
        if (input.quit==reset){

            input.force_x=0;
            input.force_y=0;
            kill(server_pid, SIGUSR2);
            kill(DroneDynamicsManager, SIGUSR2);
            kill(Obstcales_Generator, SIGUSR2);
            kill(Targets_Generator, SIGUSR2);
            usleep(10000);
            input.quit=0;
            
        }

        

        write(fd_Keyboard_to_server, &input, sizeof(KeyboardInput));
        prev_input = input;

        refresh();
        usleep(DELAY); // Control the frame rate
    }

    endwin();
    close(fd_Keyboard_to_server);
    
    return 0;

}




