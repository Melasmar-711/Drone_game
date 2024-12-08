#ifndef DYNAMICS_FUNCTIONS_H
#define DYNAMICS_FUNCTIONS_H



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/select.h>
#include"shared.h"



#define DRONE_MASS 1.0
#define TIME_STEP (1.0 / FRAME_RATE)
#define VISCOSITY_COEFFICIENT 0.7

typedef struct {
    float x, y;
} Vector_2D;


int create_and_open_fifo(const char *fifo_name, int flags);	
Vector_2D compute_repulsion_forces(int input_x_force,int input_y_force,float drone_x, float drone_y, int num_obstacles, int obstacles[][2]) ;
Vector_2D compute_viscosity_force(float velocity_x, float velocity_y);
void enforce_geofence(ServerState *state);




#endif
