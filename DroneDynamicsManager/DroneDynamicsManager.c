#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include<math.h>

#define DELAY 500000 // 500ms delay

static inline int create_and_open_fifo(const char *fifo_name, int flags) {
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("Failed to create FIFO");
        exit(1);
    }

    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    return fd;
}



void compute_repulsion_forces(int drone_x_pos,int drone_y_pos,int num_obstacles, int obstacles[][2],int* force_x,int*force_y)
{
for (int i =0;i<num_obstacles;i++){

    int dx=drone_x_pos-obstacles[i][0];
    int dy =drone_y_pos-obstacles[i][1];
    double distance=sqrt(dx*dx+dy*dy);

    if(distance<0.5){
        double repulsion = 0.1 / (distance * distance);
        *force_x+=repulsion*dx/distance;
        *force_y+=repulsion*dy/distance;
    }   

}

}





int main() {

    int fd_Dynamics_to_server = CREATE_AND_OPEN_FIFO("/tmp/DroneDynamics_to_server", O_WRONLY);
    int fd_server_to_Dynamics = CREATE_AND_OPEN_FIFO("/tmp/server_to_DroneDynamics", O_RDONLY);


    while(1)
    {

        //here you should receive the server message










        //store the last drone state

    }


    // Close pipes
    close(fd_Dynamics_to_server);
    close(fd_server_to_Dynamics);

    return 0;
}
