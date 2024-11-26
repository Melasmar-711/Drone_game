#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DRONE_MASS 1.0
#define FRAME_RATE 5
#define TIME_STEP (1.0 / FRAME_RATE)
#define MAX_X 50
#define MAX_Y 30
#define VISCOSITY_COEFFICIENT 0.7
#define MAX_OBSTACLES 10
#define MAX_TARGETS 10



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
    float x, y;
} Vector;

int create_and_open_fifo(const char *fifo_name, int flags) {
    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }
    return fd;
}

Vector compute_repulsion_forces(int input_x_force,int input_y_force,float drone_x, float drone_y, int num_obstacles, int obstacles[][2]) {
    Vector rep_force = {0, 0};

    for (int i = 0; i < num_obstacles; i++) {
        int dx = drone_x - obstacles[i][0];
        int dy = drone_y - obstacles[i][1];
        double distance = sqrt(dx * dx + dy * dy);

        if (distance > 0 && distance < 4.0) {
            if (input_x_force>1 || input_y_force>1){
            double repulsion = input_x_force*4 / (distance * distance);
            rep_force.x += repulsion * dx / distance;
            rep_force.y += repulsion * dy / distance;

            }
            else
            {
            double repulsion = 1 / (distance * distance);
            rep_force.x += repulsion * dx / distance;
            rep_force.y += repulsion * dy / distance;
            }
        }
    }
    return rep_force;
}

Vector compute_viscosity_force(float velocity_x, float velocity_y) {
    Vector viscosity = {-VISCOSITY_COEFFICIENT * velocity_x, -VISCOSITY_COEFFICIENT * velocity_y};
    return viscosity;
}


int main() {

    int fd_Dynamics_to_server = create_and_open_fifo("/tmp/DroneDynamics_to_server", O_WRONLY);
    int fd_server_to_Dynamics = create_and_open_fifo("/tmp/server_to_DroneDynamics", O_RDONLY);

    ServerState state = {0};
    Vector velocity = {0, 0};
    Vector acceleration = {0, 0};

    while (1) {

        ssize_t bytes = read(fd_server_to_Dynamics, &state, sizeof(ServerState));
        if (bytes <= 0) continue;

        Vector repulsion = compute_repulsion_forces(state.input_x_force,state.input_y_force,state.drone_x, state.drone_y, state.num_obstacles, state.obstacles);
        Vector viscosity = compute_viscosity_force(state.velocity_x, state.velocity_y);

        printf("repulsion %f %f\n",repulsion.x,repulsion.y);
        printf("vis %f %f\n",viscosity.x,viscosity.y);

        acceleration.x = (state.input_x_force + repulsion.x + viscosity.x) / DRONE_MASS;
        acceleration.y = (state.input_y_force + repulsion.y + viscosity.y) / DRONE_MASS;
        //printf("acceleration %f %f\n",acceleration.x,acceleration.y);

        velocity.x = state.velocity_x + acceleration.x * TIME_STEP;
        velocity.y = state.velocity_y + acceleration.y * TIME_STEP;
        printf("velocity %f %f\n",velocity.x,velocity.y);

        state.drone_x =state.drone_x + velocity.x * TIME_STEP;
        state.drone_y =state.drone_y + velocity.y * TIME_STEP;

        //printf("pos %f %f\n",state.drone_x,state.drone_y);


        state.velocity_x = velocity.x;
        state.velocity_y = velocity.y;

        if (state.drone_x <= 1){ 
            state.drone_x = 1;
            state.velocity_x =0;
            state.velocity_y=0;
        
        }
        if (state.drone_x >= MAX_X-1){ 
            state.drone_x = MAX_X-2;
            state.velocity_x =0;
            state.velocity_y=0;

        }
        if (state.drone_y <= 1) {
            state.drone_y = 1;
            state.velocity_y=0;
            state.velocity_x =0;

        }
        if (state.drone_y >= MAX_Y-1){ 
            state.drone_y = MAX_Y-2;
            state.velocity_y=0;
            state.velocity_x =0;
            
        }

        write(fd_Dynamics_to_server, &state, sizeof(ServerState));
        usleep(1000000 / FRAME_RATE);
    }

    close(fd_Dynamics_to_server);
    close(fd_server_to_Dynamics);
    return 0;
}
