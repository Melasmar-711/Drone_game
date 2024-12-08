#include "Generator_functions.h"
#include "sig_handle.h"



int main() {
    // Seed random number generator
    srand(time(NULL) + 1);
    signal(SIGUSR2, handle_reset_signal);
    signal(SIGINT, handle_stop_signal);



    // Create FIFO
    int fd_target_generator_to_server = create_and_open_fifo("/tmp/target_generator_to_server", O_WRONLY);

    int num_targets = MAX_TARGETS; 
    int targets[MAX_TARGETS][2];

    start:
    reset=false;




    // Generate random targets within specified boundaries
    for (int i = 0; i < num_targets; i++) {
        targets[i][0] = rand() % (MAX_X - 2) + 1;  // X coordinate
        targets[i][1] = rand() % (MAX_Y - 2) + 1;  // Y coordinate
    }

    // Send the targets array to the server
    ssize_t bytes_written = write(fd_target_generator_to_server, targets, sizeof(targets));
    if (bytes_written == -1) {
        perror("Error writing to FIFO");
        close(fd_target_generator_to_server);
        return 1;
    }

    printf("Generated and sent %d targets.\n", num_targets);


    while(!reset & !stop ){

    }
    if(reset)
    {
    goto start ;
    }

    close(fd_target_generator_to_server);
    return 0;
}