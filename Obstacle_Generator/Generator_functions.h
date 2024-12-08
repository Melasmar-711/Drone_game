#ifndef GENERATOR_FUNCTIONS_H
#define GENERATOR_FUNCTIONS_H






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
#include "shared.h"



#define MAX_OBSTACLES 10
#define MAX_TARGETS 10
#define MAX_X 100
#define MAX_Y 30
#define FRAME_RATE 30


// Data Structures
typedef struct {
    int x;
    int y;
} Target;


int create_and_open_fifo(const char *fifo_name, int flags);

#endif // GENERATOR_FUNCTIONS_H

