


#include "Dynamics_functions.h"








int create_and_open_fifo(const char *fifo_name, int flags) {
    mkfifo(fifo_name, 0666);
    int fd = open(fifo_name, flags);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }
    return fd;
}




Vector_2D compute_repulsion_forces(int input_x_force,int input_y_force,float drone_x, float drone_y, int num_obstacles, int obstacles[][2]) {
    Vector_2D rep_force = {0, 0};

    for (int i = 0; i < num_obstacles; i++) {
        int dx = drone_x - obstacles[i][0];
        int dy = drone_y - obstacles[i][1];
        double distance = sqrt(dx * dx + dy * dy);

        if (distance > 0 && distance < 4.0) {
            if (input_x_force>=1 || input_y_force>=1){
            double repulsion_x = 4*input_x_force / (distance * distance);
            double repulsion_y = 4*input_y_force / (distance * distance);

            rep_force.x += repulsion_x * dx / distance;
            rep_force.y += repulsion_y * dy / distance;

            }
            else if (input_x_force<=-1 || input_y_force<=-1){
            double repulsion_x = -4*input_x_force / (distance * distance);
            double repulsion_y = -4*input_y_force / (distance * distance);

            rep_force.x += repulsion_x * dx / distance;
            rep_force.y += repulsion_y * dy / distance;

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



Vector_2D compute_viscosity_force(float velocity_x, float velocity_y) {
    Vector_2D viscosity = {-VISCOSITY_COEFFICIENT * velocity_x, -VISCOSITY_COEFFICIENT * velocity_y};
    return viscosity;
}

