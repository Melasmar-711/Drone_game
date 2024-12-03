#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <sys/time.h>

#define MAX_OBSTACLES 10
#define MAX_TARGETS 10
#define MAX_X 100
#define MAX_Y 30
#define FRAME_RATE 30

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

typedef struct {
    int force_x;
    int force_y;
    int quit;
} KeyboardInput;

int create_and_open_fifo(const char *fifo_name, int flags);
long current_time_in_ms();
int get_max_fd(int fds[], int num_fds);






#endif // SERVER_FUNCTIONS_H

