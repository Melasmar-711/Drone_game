
#include"sig_handle.h"


volatile bool is_paused = false;



// Signal handler to toggle pause state
void handle_pause_signal(int sig) {
    if (sig == SIGUSR1) {
        is_paused = !is_paused;
        printf("Pause state toggled: %s\n", is_paused ? "Paused" : "Running");
    }
}

