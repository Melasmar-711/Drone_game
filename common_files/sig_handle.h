#ifndef SIGHANDLE
#define SIGHANDLE

#include <signal.h>
#include<stdbool.h>



extern volatile bool is_paused;


void handle_pause_signal(int sig);


#endif