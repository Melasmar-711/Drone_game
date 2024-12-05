#include "Dynamics_functions.h"


int main() {

    int fd_Dynamics_to_server = create_and_open_fifo("/tmp/DroneDynamics_to_server", O_WRONLY);
    int fd_server_to_Dynamics = create_and_open_fifo("/tmp/server_to_DroneDynamics", O_RDONLY|O_NONBLOCK);



    ServerState state = {0};
    Vector_2D velocity = {0, 0};
    Vector_2D acceleration = {0, 0};

    fd_set read_fds;
    struct timeval timeout = {0, 0};

    
    ServerState prev_state = {
        .drone_x = 10,
        .drone_y = 7,
        .input_x_force = 0,
        .input_y_force = 0,
        .resultant_force_x = 0,
        .resultant_force_y = 0,
        .velocity_x = 0,
        .velocity_y = 0,
        .num_obstacles = MAX_OBSTACLES,
        .num_targets = MAX_TARGETS,
    };


    while (1) {

        FD_ZERO(&read_fds);
        FD_SET(fd_server_to_Dynamics, &read_fds);

        
        
        int activity = select(fd_server_to_Dynamics + 1, &read_fds, NULL, NULL, &timeout);

        if(activity>0){
        ssize_t bytes = read(fd_server_to_Dynamics, &state, sizeof(ServerState));
        }
        

        else 
        {
            printf("here ");
            fflush(stdout);
   
            //prev_state.input_x_force=0;
            //prev_state.input_y_force=0;
            state=prev_state;
        }
        


        Vector_2D repulsion = compute_repulsion_forces(state.input_x_force,state.input_y_force,state.drone_x, state.drone_y, state.num_obstacles, state.obstacles);
        Vector_2D viscosity = compute_viscosity_force(state.velocity_x, state.velocity_y);


        printf("force input %d %d \n",state.input_x_force,state.input_y_force);
        printf("resultant force %f %f \n",(state.input_x_force + repulsion.x + viscosity.x) ,(state.input_y_force + repulsion.y + viscosity.y) );

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

        printf("pos %f %f\n",state.drone_x,state.drone_y);


        state.velocity_x = velocity.x;
        state.velocity_y = velocity.y;
        
        if (state.drone_x <= 1){ 
            state.drone_x = 1;
            state.velocity_x =0;
            state.velocity_y=0;
        
        }
        if (state.drone_x >= MAX_X-1){ 
            state.drone_x = MAX_X-1;
            state.velocity_x =0;
            state.velocity_y=0;

        }
        if (state.drone_y <= 1) {
            state.drone_y = 1;
            state.velocity_y=0;
            state.velocity_x =0;

        }
        if (state.drone_y >= MAX_Y){ 
            state.drone_y = MAX_Y-1;
            state.velocity_y=0;
            state.velocity_x =0;
            
        }



        write(fd_Dynamics_to_server, &state, sizeof(ServerState));
        prev_state=state;
        usleep(1000000 / FRAME_RATE);
    }

    close(fd_Dynamics_to_server);
    close(fd_server_to_Dynamics);
    return 0;
}
